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
#include "success_or_die.h"
#include "queue.h"
#include "threads.h"

#include <malloc.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct 
{
  volatile int stage  __attribute__((aligned(64)));
} counter_t;
volatile counter_t notification_ctr;


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
  //  ASSERT(provided == MPI_THREAD_FUNNELED);

  MPI_Comm_rank (MPI_COMM_WORLD, &iProc);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc);

  gaspi_rank_t iProcG, nProcG;
  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));
  SUCCESS_OR_DIE (gaspi_proc_rank (&iProcG));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProcG));
  ASSERT(iProc == iProcG);
  ASSERT(nProc == nProcG);
  
  // num threads
  omp_set_num_threads(nThreads);
  
  // assignment per proc, i-direction 
  int mSize = M_SZ/nProc;
  ASSERT(M_SZ % nProc == 0); 
  const int mStart = iProc*mSize;
  const int mStop  = (iProc+1)*mSize;

  // allocate segments for source, work
  gaspi_segment_id_t const source_id = 0;
  SUCCESS_OR_DIE ( gaspi_segment_create
                   ( source_id
                     , mSize * M_SZ * sizeof (double)
                     , GASPI_GROUP_ALL
                     , GASPI_BLOCK
                     , GASPI_MEM_UNINITIALIZED
                     ));
  gaspi_pointer_t source_array;
  SUCCESS_OR_DIE ( gaspi_segment_ptr ( source_id, &source_array) );
  ASSERT (source_array != 0);

  gaspi_segment_id_t const work_id = 1;
  SUCCESS_OR_DIE ( gaspi_segment_create
                   ( work_id
                     , mSize * M_SZ * sizeof (double)
                     , GASPI_GROUP_ALL
                     , GASPI_BLOCK
                     , GASPI_MEM_UNINITIALIZED
                     ));
  gaspi_pointer_t work_array;
  SUCCESS_OR_DIE ( gaspi_segment_ptr ( work_id, &work_array) );
  ASSERT (work_array != 0);

  // allocate target array
  ASSERT(mSize % CL == 0);
  double *target_array = memalign(CL* sizeof (double), mSize * M_SZ * sizeof (double));

  // alloc (max) M_SZ blocks per process 
  block_t (*block) = malloc( M_SZ * sizeof(block_t));	  
  int block_num = 0;

  init_global(block
	      , &block_num
	      , mSize
	      );
  
  // mutual handshake schedule - requires even nProc 
  ASSERT(nProc % 2 == 0);
  gaspi_rank_t from[nProc][nProc-1];
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

  MPI_Barrier(MPI_COMM_WORLD);

  int iter;
  double median[NITER];

  for (iter = 0; iter < NITER; iter++) 
    {

      // int notification ctr 
      notification_ctr.stage = -1;

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

#pragma omp parallel default (none) shared(iter, notification_ctr, block_num, iProc, nProc, from, \
					   block, source_array, work_array, target_array, \
					   mSize, mStart, mStop, stdout, stderr)
      {
	if (this_is_the_first_thread())
	  {
	    /*
	      TODO
	      ====
	      - Implement round-robin read on queues for alltoall
	        for a pipelined global/local transpose
	      ...
	    */
	  }

	int fnl = 0;
	do 
	  {
	    int l;
	    fnl = 0;
	    for (l = 0; l < block_num; l++) 	
	      {	    
		volatile int stage = -1;
		// check
		if ((stage = get_ctr(&block[l].stage)) < iter)
		  {	
		    // lock
		    if(omp_test_lock(&block[l].lock))
		      {
			// re-check
			if ((stage = get_ctr(&block[l].stage)) < iter)
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
				// compute off diagonal
				gaspi_notification_id_t data_available = block[l].pid;		
				gaspi_notification_id_t id;
				gaspi_return_t ret;
				if ((ret = gaspi_notify_waitsome (work_id
								  , data_available
								  , 1
								  , &id
								  , GASPI_TEST
								  )) == GASPI_SUCCESS)
				  {
				    ASSERT (id == data_available);

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

	// reset all notifications
	if (this_is_the_last_thread())
	  {
	    int k;
	    for (k = 0; k < nProc; ++k)
	      {
		if (k != iProc)
		  {		  
		    const gaspi_notification_id_t data_available = k;
		    gaspi_notification_t value;
		    SUCCESS_OR_DIE (gaspi_notify_reset (work_id
							, data_available
							, &value
							));
		    ASSERT (value == 1);
		  }
	      }
	  }	
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
  
  MPI_Barrier(MPI_COMM_WORLD);

  sort_median(&median[0], &median[NITER-1]);

  printf ("# gaspi %s nProc: %d nThreads: %d nBlocks: %d M_SZ: %d niter: %d time: %g\n"
	  , argv[0], nProc, nThreads, block_num, M_SZ, NITER, median[NITER/2]
         );
  fflush(stdout);

  MPI_Barrier(MPI_COMM_WORLD);
 
  if (iProc == nProc-1) 
    {
      double res = M_SZ*M_SZ*sizeof(double)*2 / (1024*1024*1024 * median[(NITER-1)/2]);
      printf("\nRate (Transposition Rate): %lf\n",res);
      fflush(stdout);
    }


  MPI_Finalize();

  return EXIT_SUCCESS;

}
