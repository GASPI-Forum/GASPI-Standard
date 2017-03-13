#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "testsome.h"
#include "constant.h"
#include "now.h"
#include "queue.h"


static void init_array(int *array
		       , int size
		       , gaspi_rank_t iProc
		       )
{ 
  int j;
  for (j = 0; j < size; ++j)
    {
      array[j] = -1;
    }

  /* initialize local data */
  if (iProc == 0)
    {
      for (j = 0; j < size; ++j)
	{
	  array[j] = 0;
	}
    }
}

static void validate(int *array
		     , int size
		     )
{
  int i, j;

  /* validate */
  for (j = 0; j < size; ++j)
    {	  
      ASSERT(array[j] == 0);
    }
}

static void get_next(int *next
		     , int nProc
		     , int NWAY
		     )
{
  int j, k, i = 0;
  int rnd, width = 1;

  /* 
   * set up targets for NWAY 
   * dissemination 
   */
  while (i < nProc)
    { 
      for (j = 0; j < width; j++)
        {
	  if (i+j >= nProc)
	    { 
	      break;
	    }
	  else
	    {	      
	      for (k = 0; k < NWAY; k++)
		{
		  int nx = i+width+NWAY*j+k;
		  if (nx >= nProc)
		    {
		      next[(i+j)*NWAY+k] = -1;
		    }
		  else
		    {
		      next[(i+j)*NWAY+k] = nx;
		    }	     
		}
	    }
        }
      i+=width;
      width*=NWAY;
    }
}

static void wait_and_reset(gaspi_segment_id_t segment_id
			   , gaspi_notification_id_t nid
			   , gaspi_notification_t *val
			   )
{
  gaspi_notification_id_t id;
  SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
					, nid
					, 1
					, &id
					, GASPI_BLOCK
					));
  ASSERT(nid == id);
  SUCCESS_OR_DIE(gaspi_notify_reset (segment_id
				     , id
				     , val
				     ));     
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

  /*
   * restrict NWAY dissemination, when dropping 
   * below minimal message size
   */
  int const mSize = 4096, N_SZ = 32;
  int NWAY;
  if (M_SZ < mSize)
    {
      NWAY = 7;
    }
  else if (M_SZ < mSize * N_SZ)
    {
      NWAY = 3;
    }
  else
    {
      NWAY = 1;
    }

  /* 
   * restrict number of blocks (nBlocks), when dropping 
   * below minimal message size (mSize)
   */
  int nBlocks = 512, B_SZ;
  if (M_SZ / nBlocks < mSize)
    {
      if (M_SZ > mSize)
	{
	  ASSERT(M_SZ % mSize == 0);
	  nBlocks = M_SZ / mSize;
	}
      else 
	{
	  nBlocks = 1;
	  B_SZ = M_SZ;
	}
    }
  B_SZ = M_SZ / nBlocks;
  
  /* the root of the broadcast */
  int const b_root = 0;
  int j, i;

  int next[nProc*NWAY];
  get_next(next, nProc, NWAY);

  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , M_SZ * sizeof(int)
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
  init_array(array, M_SZ, iProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI bi-directional round-robin broadcast */
  double time = -now();

  for (j = 0; j < nBlocks; ++j)
    {
      gaspi_notification_t val = 1;
      if (iProc != b_root)
	{
	  wait_and_reset(segment_id, j, &val);
	}

      for (i = 0; i < NWAY; ++i)
	{	      
	  int target = next[iProc*NWAY+i];
	  if (target != -1)
	    {		  
	      gaspi_notification_id_t notification = j;
	      gaspi_size_t b_size = B_SZ * sizeof(int);
	      gaspi_offset_t b_offset = j * b_size;
	      gaspi_queue_id_t queue = 0;
	      write_notify_and_wait ( segment_id
				      , b_offset
				      , (gaspi_rank_t) target
				      , segment_id
				      , b_offset
				      , b_size
				      , notification
				      , val
				      , (gaspi_queue_id_t) i
				      );	  
	    }
	}
    }

  time += now();
  printf("# BC  : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(array, M_SZ);

  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

