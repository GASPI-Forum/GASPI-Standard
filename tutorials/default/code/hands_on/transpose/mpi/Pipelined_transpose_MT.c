/*
 * This file is part of a small series of tutorial,
 * which aims to demonstrate key features of the GASPI
 * standard by means of small but expandable examples.
 * Conceptually the tutorial follows a MPI course
 * developed by EPCC and HLRS.
 *
 * Contact point for the MPI tutorial:
 *                 rabenseifner@hlrs.de
 * Contact point for the GASPI tutorial:
 *                 daniel.gruenewald@itwm.fraunhofer.de
 *                 mirko.rahn@itwm.fraunhofer.de
 *                 christian.simmendinger@t-systems.com
 */

#include "assert.h"
#include "constant.h"
#include "data.h"
#include "data_global.h"
#include "now.h"
#include "mm_pause.h"
#include "threads.h"

#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define DATAKEY 4712
#define USE_ALLTOALL 1

typedef struct 
{
  volatile int stage  __attribute__((aligned(64)));
} counter_t;
volatile counter_t *recv_flag;

int get_ctr(volatile int *ptr){
#pragma omp flush 
  return *ptr;
}

int increment_ctr(volatile int *ptr){
#pragma omp flush 
  __sync_fetch_and_add(ptr, 1);
#pragma omp flush 
  return *ptr;
}

int main (int argc, char *argv[])
{
  int i;
  int nProc, iProc;
  int provided, required = MPI_THREAD_FUNNELED;
  MPI_Init_thread(&argc, &argv, required, &provided);
  ASSERT(provided == MPI_THREAD_FUNNELED);

  MPI_Comm_rank (MPI_COMM_WORLD, &iProc);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc);

  // num threads
  omp_set_num_threads(nThreads);

  // assignment per proc, i-direction 
  int mSize = M_SZ/nProc;
  ASSERT(M_SZ % nProc == 0); 
  const int mStart = iProc*mSize;
  const int mStop  = (iProc+1)*mSize;

  // allocate segments for source, work, target
  ASSERT(mSize % CL == 0);
  double *source_array = memalign(CL* sizeof (double), mSize * M_SZ * sizeof (double));
  double *work_array = memalign(CL* sizeof (double), mSize * M_SZ * sizeof (double));
  double *target_array = memalign(CL* sizeof (double), mSize * M_SZ * sizeof (double));

  // alloc (max) M_SZ blocks per process 
  block_t (*block) = malloc( M_SZ * sizeof(block_t));	  
  int block_num = 0;

  // init block data 
  init_global(block
	      , &block_num
	      , mSize
	      );

  // mutual handshake schedule - requires even nProc 
  ASSERT(nProc % 2 == 0);
  int from[nProc][nProc-1];
  for (i = 0; i < nProc - 1 ; ++i)
    {
      int k;
      from[i][i] = nProc-1;
      from[nProc-1][i] = i;
      for (k = 1; k < nProc/2 ; ++k)
        {
          int d1 = (i - k + (nProc-1)) % (nProc-1);           
          int d2 = (i + k + (nProc-1)) % (nProc-1);           
          from[d1][i] = d2;
          from[d2][i] = d1;
        }
    }     

  // init recv flags for multithreaded local transpose
  recv_flag = malloc((nProc) * sizeof(counter_t));
  for (i = 0; i < nProc; ++i)
    {
      recv_flag[i].stage = -1;
    }


  MPI_Barrier(MPI_COMM_WORLD);

  int iter;
  double median[NITER];
  for (iter = 0; iter < NITER; iter++) 
    {

      // init thread local data, set thread range (tStart <= row <= tStop)
      data_init_global(mStart
		       , mStop
		       , block
		       , block_num
		       , source_array
		       , work_array
		       , target_array
		       , mSize
		       );


      double time = -now();
      MPI_Barrier(MPI_COMM_WORLD);

#pragma omp parallel default (none) shared(block_num, iProc, nProc, from, recv_flag, \
					   iter, block, source_array, work_array, target_array, \
					   mSize, mStart, mStop, stdout, stderr)
      {
	int const tid = omp_get_thread_num();  

#ifdef USE_ALLTOALL
        if (tid == 0)
	  {
	    int k;
	    MPI_Alltoall(source_array
			 , mSize*mSize
			 , MPI_DOUBLE
			 , work_array
			 , mSize*mSize
			 , MPI_DOUBLE
			 , MPI_COMM_WORLD
			 );
	    for (k = 0; k < nProc-1; ++k)
	      {
		// flag received buffer 
		const int source = from[iProc][k];
		increment_ctr(&recv_flag[source].stage);		
	      }
	  }
#else

	if (tid == 0)
	  {
	    /*
	      TODO
	      ====
	      - Implement global transpose with 2-sided MPI communication instead of alltoall
	      for a pipelined global/local transpose
	      ...
	    */
	  }
#endif

	int fnl = 0;
	do 
	  {
	    int l;
	    fnl = 0;
	    for (l = 0; l < block_num; l++) 	
	      {	    
		volatile int block_stage = -1;
		// check
		if ((block_stage = get_ctr(&block[l].stage)) < iter)
		  {	
		    // lock
		    if(omp_test_lock(&block[l].lock))
		      {
			// re-check
			if ((block_stage = get_ctr(&block[l].stage)) < iter)
			  {
			    // compute (node) local diagonal 
			    if (block[l].pid == iProc)
			      {
				data_compute(mStart
					     , mStop
					     , block
					     , l
					     , source_array
					     , target_array
					     , mSize
					     );

				//increment ctr
				increment_ctr(&block[l].stage);
			      }
			    else
			      {
				int pid = block[l].pid;
				volatile int recv_stage = -1;
				
				// check
				if ((recv_stage = get_ctr(&recv_flag[pid].stage)) == iter)
				  {	
				    // compute off diagonal
				    data_compute(mStart
						 , mStop
						 , block
						 , l
						 , work_array
						 , target_array
						 , mSize
						 );

				    //increment ctr
				    increment_ctr(&block[l].stage);

				  }			  
			      }
			  }
			// unlock
			omp_unset_lock (&block[l].lock);
		      }
		  }
		else
		  {
		    ASSERT(block[l].stage == iter);
		    fnl++;
		  }
	      }	    
	  }
	while (fnl < block_num);


      }

      MPI_Barrier(MPI_COMM_WORLD);
      time += now();

      /* iteration time */
      median[iter] = time;

      // validate */ 
      data_validate(mSize
                    , mStart
                    , target_array
                    );

    }


  sort_median(&median[0], &median[NITER-1]);

  printf ("# mpi %s nProc: %d nThreads: %d nBlocks: %d M_SZ: %d niter: %d time: %g\n"
	  , argv[0], nProc, nThreads, block_num, M_SZ, NITER, median[NITER/2]
         );

  MPI_Barrier(MPI_COMM_WORLD);
 
  if (iProc == nProc-1) 
    {
      double res = M_SZ*M_SZ*sizeof(double)*2 / (1024*1024*1024 * median[(NITER-1)/2]);
      printf("\nRate (Transposition Rate): %lf\n",res);
    }

  MPI_Finalize();


  return EXIT_SUCCESS;

}
