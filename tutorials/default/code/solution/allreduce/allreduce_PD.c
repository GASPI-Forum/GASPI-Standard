#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "testsome.h"
#include "util.h"
#include "queue.h"

static void init_array(int *array
		       , int size
		       , int nstage
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j;
  for (j = 0; j < size; ++j)
    {
      array[j] = iProc;
    }
  for (i = 1; i < nstage + 1; ++i)
    {
      for (j = 0; j < size; ++j)
	{
	  array[i*size + j] = -1;
	}
    }
}

static void validate(int *array
		     , int size
		     , int nstage
		     , gaspi_rank_t iProc
		     , gaspi_rank_t nProc
		     )
{

  /* validate */
  int i, j;
  int sum = 0;
  for (j = 0; j < nProc; ++j)
    {
      sum += j;
    }
  for (i = 0; i < size; ++i)
    {
      ASSERT(array[nstage*size + i] == sum);
    }
}

int
main (int argc, char *argv[])
{  

  int nProc_MPI, iProc_MPI;
  gaspi_rank_t iProc, nProc;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &iProc_MPI);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc_MPI);

  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));
  SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProc));

  ASSERT(iProc == iProc_MPI);
  ASSERT(nProc == nProc_MPI);

  /* restrict number of blocks */
  int nBlocks = 512, mSize = 4096;
  restrict_nBlocks(&nBlocks, &mSize, M_SZ);

  int const max_stage = 32;
  int nstage = 0, istage[max_stage], rstage[max_stage];
  int istep[max_stage], ioffset[max_stage], itarget[nProc];
  get_offsets(istep
	      , itarget
	      , ioffset
	      , istage
	      , rstage
	      , &nstage
	      , iProc
	      , nProc
	      );

  /* stage counter per block */
  int i, recv_stage[nBlocks];
  for (i = 0; i < nBlocks; ++i)
    {
      recv_stage[i] = 0;
    }

  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , (nstage + 1) * M_SZ * sizeof(int)
					 , GASPI_GROUP_ALL
					 , GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
		  );
  
  gaspi_pointer_t _ptr = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id, &_ptr));

  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));

  int *array = (int *) _ptr;
  init_array(array, M_SZ, nstage, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  for (i = 0; i < nBlocks; ++i)
    {
      int sz = (i == nBlocks - 1) ? M_SZ - i * mSize : mSize;
      gaspi_rank_t target = itarget[0];
      gaspi_notification_id_t notification = i * nstage;
      gaspi_size_t b_size = sz * sizeof(int);
      gaspi_offset_t l_offset =  i * mSize * sizeof(int);
      gaspi_offset_t r_offset =  (M_SZ + i * mSize) * sizeof(int);      

      write_notify_and_wait ( segment_id
			      , l_offset
			      , target
			      , segment_id
			      , r_offset
			      , b_size
			      , notification
			      , 1
			      , target % queue_num
			      );
    }

  int recv_count = 0;
  for (i = 0; recv_count < nstage * nBlocks; i = (i + 1) % nBlocks)
    {
      gaspi_notification_id_t nid = nstage * i + recv_stage[i];   
      
      /* test for incoming messages */
      if (recv_stage[i] < nstage && test_or_die(segment_id, nid, 1))
        {
	  /* increment stage .. */
	  int stage = recv_stage[i] + 1;

	  /* .. and reduce */
	  int sz = (i == nBlocks - 1) ? M_SZ - i * mSize : mSize;
	  int offset =  stage * M_SZ + i * mSize;
	  reduce(array, offset, sz, M_SZ, iProc);
	  
	  /* increase, block, recv counter */
	  recv_stage[i]++;
	  recv_count++;

	  if (stage < nstage)
	    {
	      gaspi_rank_t target = itarget[stage];
	      gaspi_notification_id_t notification = i * nstage + stage;
	      gaspi_size_t b_size = sz * sizeof(int);

	      /* re-use partial reduce results */
	      gaspi_offset_t l_offset =  (istage[stage] * M_SZ + i * mSize) * sizeof(int);
	      gaspi_offset_t r_offset =  (rstage[stage] * M_SZ + i * mSize) * sizeof(int);      

	      write_notify_and_wait ( segment_id
				      , l_offset
				      , target
				      , segment_id
				      , r_offset
				      , b_size
				      , notification
				      , 1
				      , target % queue_num
				      );
	    }	 
	}
    }  

  time += now();
  printf("# RR  : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(array, M_SZ, nstage, iProc, nProc);
      
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

