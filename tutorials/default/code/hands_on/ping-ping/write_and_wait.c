#include <stdlib.h>
#include <GASPI.h>
#include <omp.h>
#include "assert.h"
#include "success_or_die.h"
#include "constant.h"
#include "queue.h"

static int *send_count = NULL;
  
int
main (int argc, char *argv[])
{  
  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t iProc;
  gaspi_rank_t nProc;

  SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProc));
  ASSERT (nProc == 2);

  omp_set_num_threads(nThreads);
  gaspi_rank_t target = 1 - iProc;  

  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));


  int const M_SZ = 1048576;
  int const B_SZ = 4;
  
  int   elem_size[queue_num];
  send_count = malloc(queue_num * sizeof(int));
  ASSERT(send_count != NULL);
		 
  int vlen = 0;
  int i, j, k = 1;
  for (i = 0; i < queue_num; ++i)
    {
      send_count[i] = 0;
      elem_size[i] = k;
      ASSERT(k <= M_SZ);
      ASSERT(M_SZ % k == 0);      
      k *= B_SZ;
    }
  
  const gaspi_segment_id_t segment_id_src = 0;
  const gaspi_segment_id_t segment_id_dst = 1;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_src
					 , queue_num * M_SZ * sizeof(int)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
		  );
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_dst
					 , queue_num * M_SZ * sizeof(int)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
		  );
  
  gaspi_pointer_t _src_ptr = NULL;
  gaspi_pointer_t _dst_ptr = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_src, &_src_ptr));
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_dst, &_dst_ptr));

  int *src = (int *) _src_ptr;
  int *dst = (int *) _dst_ptr;

  for (i = 0; i < queue_num; ++i)
    {
      for (j = 0; j < M_SZ; ++j)
	{
	  dst[i * M_SZ + j] = -1;
	  src[i * M_SZ + j] =  i;
	}
    }

  /* TODO: Explain: Why do we need the barrier ? */
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));
  
  
#pragma omp parallel for 
  for (i = 0; i < queue_num * M_SZ; ++i)
    {
      int id =  i / M_SZ;

      /* 
       * in queue 'id', write 'M_SZ/elem_size[id]' chunks 
       * of 'elem_size[id]' integers 
       */
      if (i % elem_size[id] == 0)
	{      
	  gaspi_offset_t offset  = i * sizeof(int);
	  gaspi_size_t size = elem_size[id] * sizeof(int);

	  /* 
	   * TODO: write chunks per designated queue_id.
	   * Wait wherever reuqired. (Hint: use write_and_wait) 
	   */


	  
	  /* 
	   * notify complete queues, use 
	   * send_count as notification value 
	   */
	  if (__sync_add_and_fetch
	      (&send_count[id], 1) == M_SZ/elem_size[id])
	    {
	      notify_and_wait (segment_id_dst
			       , target
			       , (gaspi_notification_id_t) id
			       , send_count[id]
			       , (gaspi_queue_id_t) id
			       );
	    }
	}
    }
  
  int received = 0;
  while (received < queue_num)
    {
      gaspi_notification_id_t id;
      SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id_dst
					    , 0
					    , queue_num
					    , &id
					    , GASPI_BLOCK
					    ));
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id_dst
					 , id
					 , &value
					 ));
      
      ASSERT(elem_size[id] * value == M_SZ);
      for (j = 0; j < M_SZ; ++j)
	{
	  ASSERT (dst[id * M_SZ +j] == (int) id);
	}

      printf("# queue: %8d message size: %8d send_count: %8d\n"
	     , id, elem_size[id],value);
      fflush(stdout);
	  
      received++;
    }

  
  wait_for_flush_queues();
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

