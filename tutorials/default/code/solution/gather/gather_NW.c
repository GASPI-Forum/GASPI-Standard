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


/* initialize local data */
static void init_array(int *array
		       , int size
		       , gaspi_rank_t nProc
		       , gaspi_rank_t iProc
		       )
{ 
  int j;
  for (j = 0; j < size; ++j)
    {
      array[j] = iProc;
    }
  for (j = size; j < nProc*size; ++j)
    {
      array[j] = -1;
    }
}

/*
 * validate solution 
 */
static void validate(int *array
		     , int size
		     , gaspi_rank_t nProc
		     , gaspi_rank_t iProc
		     )
{
  int i, j;
  if (iProc == 0)
    {
      for (j = 0; j < nProc; ++j)
	{
	  for (i = 0; i < size; ++i)
	    {
	      ASSERT(array[j*size+i] == j);
	    }
	}
    }
}

/*
 * restrict NWAY dissemination, when dropping 
 * below minimal message size
 */
static void restrict_NWAY(int *NWAY
			  , int mSize
			  , int N_SZ
			  )
{
  if (M_SZ < mSize)
    {
      *NWAY = 9;
    }
  else if (M_SZ < mSize * N_SZ)
    {
      *NWAY = 5;
    }
  else
    {
      *NWAY = 2;
    }
}

/* 
 * restrict number of blocks (nBlocks), when dropping 
 * below minimal message size (mSize)
 */
static void restrict_nBlocks(int *nBlocks
			     , int *mSize
			     , int *lSize
			     )
{
  if (M_SZ < *mSize * *nBlocks)
    {
      /* reduce number of blocks */
      if (M_SZ > *mSize)
	{
	  *nBlocks = M_SZ / *mSize;
	  *lSize = M_SZ % *mSize;
	  if (*lSize != 0)
	    {
	      (*nBlocks)++;
	    }
	  else
	    {
	      *lSize = *mSize;
	    } 
	}
      else
	{
	  *nBlocks = 1;
	  *lSize = M_SZ;
	}
    }
  else
    {
      /* increase message size */
      *mSize = M_SZ / *nBlocks;
      *lSize = M_SZ % *mSize;
      if (*lSize != 0)
	{
	  (*nBlocks)++;
	}
      else
	{
	  *lSize = *mSize;
	} 
    }
  ASSERT((*nBlocks - 1) * *mSize + *lSize == M_SZ);
}

static int get_n_next(int idx
		      , int NWAY
		      , int (*next)[NWAY]
		      )
{
  int k, i = 1;
  for (k = 0; k < NWAY; k++)
    {
      int j = next[idx][k];
      if (j != -1)
	{
	  i +=  get_n_next(j, NWAY, next); 
	}
    }
  return i;
}
/* 
 * set up source and target for NWAY 
 * dissemination 
 */
static void get_p_next(int nProc
		       , int NWAY
		       , int *prev
		       , int (*next)[NWAY]
		       , int *nRecv
		       )
{
  int j, k, i = 0;
  int rnd, width = 1;

  for (j = 0; j < nProc; j++)
    {
      prev[j] = -1;
      for (k = 0; k < NWAY; k++)
	{
	  next[j][k] = -1;
	}
    }

  /* set prev, next */
  while (i < nProc)
    { 
      for (j = 0; j < width; j++)
        {
	  for (k = 0; k < NWAY; k++)
	    {
	      int nx = i+width+NWAY*j+k;
	      int ix = i+j;
	      if (nx < nProc && ix < nProc )
		{
		  ASSERT(prev[nx] == -1);
		  next[ix][k] = nx;
		  prev[nx] = ix;
		}
	    }
        }
      i += width;
      width *= NWAY;
    }

  /* number of incoming messages */
  for (j = 0; j < nProc; j++)
    {
      nRecv[j] =  get_n_next(j, NWAY, next) - 1; 
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

  /* restric NWAY dissemination */
  int NWAY = 2, mSize = 4096, N_SZ = 64;
  restrict_NWAY(&NWAY, mSize, N_SZ);

  /* restrict number of blocks */
  int nBlocks = 511, lSize = 0;
  restrict_nBlocks(&nBlocks, &mSize, &lSize);
  
  /* the root of the gather */
  int const b_root = 0;
  int j, k, i;

  /* pointer of incoming recv */
  int recv_state[nProc];
  for (i = 0; i < nProc; ++i)
    {
      recv_state[i] = 0;
    }
  
  int prev[nProc], nRecv[nProc];
  int next[nProc][NWAY];
  get_p_next(nProc, NWAY, prev, next, nRecv);
  
  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , nProc * M_SZ * sizeof(int)
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
  init_array(array, M_SZ, nProc, iProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI pipelined NWAY gather */
  double time = -now();

  int target = prev[iProc];
  if (target != -1)
    {
      for (j = 0; j < nBlocks; ++j)
	{
	  int sz = (j == nBlocks-1) ? lSize : mSize;
	  gaspi_notification_id_t notification = iProc * nBlocks + j;
	  gaspi_size_t b_size = sz * sizeof(int);
	  gaspi_offset_t l_offset =  j * mSize * sizeof(int);
	  gaspi_offset_t r_offset = (j * mSize + iProc * M_SZ) * sizeof(int);
	  write_notify_and_wait ( segment_id
				  , l_offset
				  , (gaspi_rank_t) target
				  , segment_id
				  , r_offset
				  , b_size
				  , notification
				  , 1
				  , target % queue_num
				  );	  
	}
    }

  int recv_count = 0;
  for (i = 0; recv_count < nRecv[iProc] * nBlocks; i = (i + 1) % nProc)
    {
      gaspi_notification_id_t nid = recv_state[i] + i * nBlocks;   
      gaspi_notification_t val;
      
      /* test for incoming messages, forward if required */
      if (recv_state[i] < nBlocks && test_or_die(segment_id, nid, &val))
	{
	  ASSERT(val == 1);

	  gaspi_rank_t source = nid / nBlocks;
	  gaspi_notification_id_t sid = nid % nBlocks;	  
	  ASSERT(source == i);

	  /* increment recv counter .. */
	  recv_count++;
	  recv_state[i]++;
	  
	  /* .. and forward */
	  if (target != -1)
	    {
	      int sz = (sid == nBlocks-1) ? lSize : mSize;
	      gaspi_notification_id_t notification = nid;
	      gaspi_size_t b_size = sz * sizeof(int);
	      gaspi_offset_t b_offset = (source * M_SZ + sid * mSize) * sizeof(int);
	      write_notify_and_wait ( segment_id
				      , b_offset
				      , (gaspi_rank_t) target
				      , segment_id
				      , b_offset
				      , b_size
				      , notification
				      , 1
				      , target % queue_num
				      );	  
	    }
	}
    }

  time += now();
  printf("# BC  : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(array, M_SZ, nProc, iProc);

  wait_for_flush_queues();
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  MPI_Finalize();
  
  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

