#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "now.h"
#include "queue.h"

static void init_array(int *array
		       , gaspi_offset_t *offset
		       , gaspi_size_t *size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < size[i]; ++j)
	{
	  array[offset[i] + j] = -1;
	}
    }

  /* initialize local data */
  for (j = 0; j < size[iProc]; ++j)
    {
      array[offset[iProc] + j] = iProc;
    }
}

static void validate(int *array
		       , gaspi_offset_t *offset
		       , gaspi_size_t *size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		     )
{
  int i, j;

  /* validate */
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < size[i]; ++j)
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

  int const B_SZ = 2;
  int const M_SZ = 2048;

  int *received = malloc(nProc * sizeof(int));
  ASSERT(received != NULL);

  gaspi_offset_t *offset = malloc(nProc * sizeof(gaspi_offset_t));
  ASSERT(offset != NULL);  

  gaspi_size_t  *size = malloc(nProc * sizeof(gaspi_size_t));
  ASSERT(size != NULL);
    
  int vlen = 0;
  int i, j, k = 1;
  
  for (i = 0; i < nProc; ++i)
    {
      received[i] = 0;
      offset[i]   = vlen;
      size[i]     = M_SZ * k;
      vlen       += size[i];
      k          *= B_SZ;
    }
  received[iProc] = 1;
  
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
  init_array(array, offset, size, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  /* 
   * TODO - start the round-robin data shuffle 
   * via write_notify_and_wait (see queue.c)
   */


  
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

      /* 
       * TODO - Why do we have do keep track of 
       * received id's here ?
       */

      if (!received[id] )
	{
	  if (id != RIGHT(iProc, nProc))
	    {
	      notification = id;
	      b_size = size[id] * sizeof(int);
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
	  received[id] = 1;
	}
    }

  time += now();
  printf("# RR  : iProc: %4d, size [byte]: %10lu, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, size[iProc], time, (double)(vlen*sizeof(int))/1024/1024/time); 

  /*
   * TODO: 
   * Why can we achive "bandwidth" numbers which actually exceed 
   * the theoretically possible hardware limit ?
   * 
   * Why are the timings in this algorithm rather unbalanced ?
   * How can we remove this imbalance ?
   */

  validate(array, offset, size, iProc, nProc);
      
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  MPI_Finalize();

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

