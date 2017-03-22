#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "util.h"
#include "testsome.h"
#include "queue.h"

static void init_array(int *array
		       , gaspi_offset_t *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j, k;
  for (i = 0; i < nProc; ++i)
    {
      for (k = 0; k < bSize[i]; ++k)
	{
	  array[offset[i] + k] = -1;
	}
    }

  /* initialize local data */
  for (k = 0; k < bSize[iProc]; ++k)
    {
      array[offset[iProc] + k] = iProc;
    }
}

static void validate(int *array
		       , gaspi_offset_t *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		     )
{
  int i, j, k;

  /* validate */
  for (i = 0; i < nProc; ++i)
    {
      for (k = 0; k < bSize[i]; ++k)
	{
	  ASSERT(array[offset[i] + k] == i);
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
 
  int i, j, vlen = 0, nBlocksTotal = 0;

  /* init recv state */ 
  int recv_state[nProc];
  init_recv_state(recv_state, nProc);


  /* init size */ 
  int nBlocks[nProc];
  int mSize[nProc];
  init_block_data(nBlocks, mSize, nProc);


  /* init length, offset  */ 
  int bSize [nProc];
  gaspi_offset_t offset[nProc];    
  srand(0);
  for (i = 0; i < nProc; ++i)
    {
      int rSize = rand() % M_SZ;
      restrict_nBlocks(&nBlocks[i]
		       , &mSize[i]
		       , rSize
		       );
      bSize[i] = rSize;
      nBlocksTotal += nBlocks[i];
      offset[i] = vlen;
      vlen += rSize;
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
  init_array(array, offset, bSize, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  for (j = 0; j < nBlocks[iProc]; ++j)
    {
      int sz = (j == nBlocks[iProc] - 1) ? bSize[iProc] - j*mSize[iProc] : mSize[iProc];
      gaspi_notification_id_t notification = iProc*nBlocksMax+j;
      gaspi_size_t b_size = sz * sizeof(int);
      gaspi_offset_t b_offset = (offset[iProc] + mSize[iProc]*j) * sizeof(int);
      gaspi_rank_t target = RIGHT(iProc, nProc);
      write_notify_and_wait ( segment_id
			      , b_offset
			      , target
			      , segment_id
			      , b_offset
			      , b_size
			      , notification
			      , 1
			      , 0
			      );
    }
  
  /* receive and write until done */
  int recv_count = 0;
  for (i = 0; recv_count < nBlocksTotal - nBlocks[iProc]; i = (i + 1) % nProc)
    {
      gaspi_notification_id_t nid = recv_state[i] + i * nBlocksMax;   
      
      /* test for incoming messages, forward if required */
      if (recv_state[i] < nBlocks[i] && test_or_die(segment_id, nid, 1))
        {
          gaspi_rank_t source = nid / nBlocksMax;
          gaspi_notification_id_t sid = nid % nBlocksMax;    

          ASSERT(source == i);
          ASSERT(sid == recv_state[i]);

          /* increment recv counter .. */
          recv_count++;
          recv_state[i]++;
          
          /* .. and forward */
	  if (source != RIGHT(iProc, nProc))
	    {
	      gaspi_notification_id_t notification = nid;
	      int sz = (sid == nBlocks[source] - 1) ? bSize[source] - sid*mSize[source] : mSize[source];
	      gaspi_size_t b_size = sz * sizeof(int);
	      gaspi_offset_t b_offset = (offset[source] + mSize[source]*sid) * sizeof(int);
	      gaspi_rank_t target = RIGHT(iProc, nProc);
	      write_notify_and_wait ( segment_id
				      , b_offset
				      , target
				      , segment_id
				      , b_offset
				      , b_size
				      , notification
				      , 1
				      , source % queue_num
				      );
	    }
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

