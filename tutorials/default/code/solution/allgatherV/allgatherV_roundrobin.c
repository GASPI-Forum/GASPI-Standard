#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "queue.h"

static void init_array(int *array
		       , gaspi_offset_t *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < bSize[i]; ++j)
	{
	  array[offset[i] + j] = -1;
	}
    }

  /* initialize local data */
  for (j = 0; j < bSize[iProc]; ++j)
    {
      array[offset[iProc] + j] = iProc;
    }
}

static void validate(int *array
		       , gaspi_offset_t *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		     )
{
  int i, j;

  /* validate */
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < bSize[i]; ++j)
	{	  
	  ASSERT(array[offset[i] + j] == i);
	}
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

  int *recv_state = malloc(nProc * sizeof(int));
  ASSERT(recv_state != NULL);

  gaspi_offset_t *offset = malloc(nProc * sizeof(gaspi_offset_t));
  ASSERT(offset != NULL);  

  int  *bSize = malloc(nProc * sizeof(int));
  ASSERT(bSize != NULL);
    
  int i, vlen = 0;
  srand(0);
  for (i = 0; i < nProc; ++i)
    {
      int rSize = rand() % M_SZ;
      offset[i]   = vlen;
      bSize[i]     = rSize;
      vlen       += bSize[i];
    }

  for (i = 0; i < nProc; ++i)
    {
      recv_state[i] = 0;
    }
  recv_state[iProc] = 1;
  
  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , vlen * sizeof(int)
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
  init_array(array, offset, bSize, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  gaspi_notification_id_t notification = iProc;
  gaspi_size_t b_size = bSize[iProc] * sizeof(int);
  gaspi_offset_t b_offset = offset[iProc] * sizeof(int);
  gaspi_rank_t target = RIGHT(iProc, nProc);
  write_notify_and_wait ( segment_id
			  , b_offset
			  , target
			  , segment_id
			  , b_offset
			  , b_size
			  , notification
			  , 1
			  , iProc % queue_num
			  );
  
  /* receive and write until done */
  for (i = 0; i < nProc - 1; ++i)
    {
      gaspi_notification_id_t id;
      SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
					    , 0
					    , nProc
					    , &id
					    , GASPI_BLOCK
					    ));
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id
					 , id
					 , &value
					 ));     
      ASSERT(value == 1);
      
      if (!recv_state[id] )
	{
	  if (id != RIGHT(iProc, nProc))
	    {
	      notification = id;
	      b_size = bSize[id] * sizeof(int);
	      b_offset = offset[id] * sizeof(int);
	      target = RIGHT(iProc, nProc);
	      write_notify_and_wait ( segment_id
				      , b_offset
				      , target
				      , segment_id
				      , b_offset
				      , b_size
				      , notification
				      , 1
				      , id % queue_num
				      );
	    }
	  recv_state[id] = 1;
	}
    }

  time += now();
  printf("# RR  : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, bSize[iProc], time, (double)(vlen*sizeof(int))/1024/1024/time); 
  
  validate(array, offset, bSize, iProc, nProc);
      
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

