#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "now.h"
#include "testsome.h"
#include "queue.h"

static void init_array(int *array
		       , gaspi_offset_t *offset
		       , gaspi_size_t *size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j, k;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < nProc; ++j)
	{
	  for (k = 0; k < size[i*nProc+j]; ++k)
	    {
	      array[offset[i*nProc+j] + k] = -1;
	    }
	}
    }

  /* initialize local data */
  for (j = 0; j < nProc; ++j)
    {
      for (k = 0; k < size[iProc*nProc+j]; ++k)
	{
	  array[offset[iProc*nProc+j] + k] = iProc;
	}
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
      for (j = 0; j < nProc; ++j)
	{
	  for (k = 0; k < size[i*nProc+j]; ++k)
	    {
	      ASSERT(array[offset[i*nProc+j] + k] == i);
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

  int const B_SZ = 2;
  int const M_SZ = 2048;
  ASSERT(M_SZ % nProc == 0);

  gaspi_number_t notification_max;
  SUCCESS_OR_DIE (gaspi_notification_num(&notification_max));
  ASSERT(nProc*nProc <= notification_max);

  gaspi_offset_t *offset = malloc(nProc*nProc*sizeof(gaspi_offset_t));
  ASSERT(offset != NULL);
  
  gaspi_size_t  *size = malloc(nProc*nProc*sizeof(gaspi_size_t));
  ASSERT(size != NULL);
    
  int vlen = 0;
  int i, j, k = 1;  
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < nProc; ++j)
	{
	  offset[i*nProc+j] = vlen;
	  size[i*nProc+j]   = M_SZ/nProc * k;
	  vlen             += size[i*nProc+j];
	}
      k *= B_SZ;
    }

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

  /* GASPI - scatter all local arrays */
  double time = -now();
  for (i = 0; i < nProc; ++i)
    {
      if (i != iProc)
	{
	  /* distribute .. */
	  gaspi_notification_id_t notification = iProc*nProc + i;
	  gaspi_size_t b_size = size[notification];
	  gaspi_offset_t b_offset = offset[notification];
	  gaspi_rank_t target = i;
	  write_notify_and_wait ( segment_id
				  , b_offset * sizeof(int)
				  , target
				  , segment_id
				  , b_offset * sizeof(int)
				  , b_size  * sizeof(int)
				  , notification
				  , 1
				  , i % queue_num
				  );
	}      
      else
	{
	  /* .. notify self */
	  gaspi_notification_id_t notification = iProc*nProc + i;
	  gaspi_rank_t target = i;
	  notify_and_wait ( segment_id
			    , target
			    , notification
			    , 1
			    , i % queue_num
			    );
	}      
    }


  /* ... and re-distribute */
  int received = 0;
  while (received < nProc )
    {
      for (i = iProc; i < nProc*nProc; i+=nProc)
	{
	  if ( test_or_die(segment_id, i, 1)) 
	    {
	      int rProc = i/nProc;
	      for (j = 0; j < nProc; ++j)
		{
		  if (j != rProc && j != iProc)
		    {
		      gaspi_notification_id_t notification = i;
		      gaspi_size_t b_size = size[notification];
		      gaspi_offset_t b_offset = offset[notification];
		      gaspi_rank_t target = j;
		      write_notify_and_wait ( segment_id
					      , b_offset * sizeof(int)
					      , target
					      , segment_id
					      , b_offset * sizeof(int)
					      , b_size * sizeof(int)
					      , notification
					      , 1
					      , j % queue_num
					      );
		    }
		}
	      received++;
	    }
	}
    }

  /* wait until complete */
  received = 0;
  while (received < (nProc-1)*(nProc-1))
    {
      for (i = 0; i < nProc*nProc; i++)
	{
	  if ( test_or_die(segment_id, i, 1) ) 
	    {
	      ASSERT(i % nProc != iProc);
	      received++;
	    }
	}
    }

  time += now();
  printf("# PL  : iProc: %4d, size [byte]: %10lu, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, size[iProc*nProc]*nProc, time, (double)(vlen*sizeof(int))/1024/1024/time); 
  
  validate(array, offset, size, iProc, nProc);      

  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

