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
		       , int *received
		       , gaspi_offset_t *offset
		       , gaspi_size_t *size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j, k;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < NWAY; ++j)
	{
	  for (k = 0; k < size[i*NWAY+j]; ++k)
	    {
	      array[offset[i*NWAY+j] + k] = -1;
	    }
	}
    }

  /* initialize local data */
  for (j = 0; j < NWAY; ++j)
    {
      for (k = 0; k < size[iProc*NWAY+j]; ++k)
	{
	  array[offset[iProc*NWAY+j] + k] = iProc;
	}
    }

  /* initialize receive status for all data chunks */ 
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < NWAY; ++j)
	{
	  received[i*NWAY+j] = 0;
	}
    }
  for (j = 0; j < NWAY; ++j)
    {
      received[iProc*NWAY+j] = 1;
    }
  
}

static void validate(int *array
		       , gaspi_offset_t *offset
		       , gaspi_size_t *size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		     )
{
  int i, j, k;

  /* validate */
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < NWAY; ++j)
	{
	  for (k = 0; k < size[i*NWAY+j]; ++k)
	    {
	      ASSERT(array[offset[i*NWAY+j] + k] == i);
	    }
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
  ASSERT(nProc % NWAY == 0);
  
  int *received = malloc(nProc * NWAY * sizeof(int));
  ASSERT(received != NULL);

  gaspi_offset_t *offset = malloc(nProc * NWAY * sizeof(gaspi_offset_t));
  ASSERT(offset != NULL);  

  gaspi_size_t  *size = malloc(nProc * NWAY * sizeof(gaspi_size_t));
  ASSERT(size != NULL);
    
  int i, j, vlen = 0;
#ifdef RAND
  srand(0);
  for (i = 0; i < nProc; ++i)
    {
      int rsize= rand() % M_SZ;
      for (j = 0; j < NWAY; ++j)
        {
          offset[i*NWAY+j] = vlen;
          size[i*NWAY+j]   = rsize * nProc / NWAY;
          vlen             += size[i*NWAY+j];
        }
    }
#else
  int k = 1;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < NWAY; ++j)
        {
          offset[i*NWAY+j] = vlen;
          size[i*NWAY+j]   = M_SZ * k * nProc / NWAY;
          vlen             += size[i*NWAY+j];
        }
      k *= B_SZ;
    }
#endif  

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
  init_array(array, received, offset, size, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  for (j = 0; j < NWAY; ++j)
    {
      gaspi_notification_id_t notification = iProc*NWAY+j;
      gaspi_size_t b_size = size[iProc*NWAY+j] * sizeof(int);
      gaspi_offset_t b_offset = offset[iProc*NWAY+j] * sizeof(int);
      gaspi_rank_t target = RIGHT(iProc, nProc);
      write_notify_and_wait ( segment_id
			      , b_offset
			      , target
			      , segment_id
			      , b_offset
			      , b_size
			      , notification
			      , 1
			      , notification % queue_num
			      );
    }
  
  /* receive and write until done */
  for (i = 0; i < NWAY * (nProc-1); ++i)
    {
      gaspi_notification_id_t id;
      SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
					    , 0
					    , nProc*NWAY
					    , &id
					    , GASPI_BLOCK
					    ));
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id
					 , id
					 , &value
					 ));     
      ASSERT(value == 1);
      gaspi_rank_t source = id / NWAY;

      if (!received[id] )
	{
	  if (source != RIGHT(iProc, nProc))
	    {
	     gaspi_notification_id_t notification = id;
	     gaspi_size_t b_size = size[id] * sizeof(int);
	     gaspi_offset_t b_offset = offset[id] * sizeof(int);
	     gaspi_rank_t target = RIGHT(iProc, nProc);
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
	  received[id] = 1;
	}
    }

  time += now();
  printf("# RR  : iProc: %4d, size [byte]: %10lu, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, size[iProc*NWAY]*NWAY, time, (double)(vlen*sizeof(int))/1024/1024/time); 
  
  validate(array, offset, size, iProc, nProc);
      
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

