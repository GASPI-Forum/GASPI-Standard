#include <stdlib.h>
#include <GASPI.h>
#include <omp.h>
#include "assert.h"
#include "success_or_die.h"
#include "constant.h"
#include "now.h"
#include "queue.h"

int
main (int argc, char *argv[])
{
  int i;
  
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
  
  gaspi_number_t notification_max;
  SUCCESS_OR_DIE (gaspi_notification_num(&notification_max));
  notification_max -= 1;
    
  const gaspi_segment_id_t segment_id_src = 0;
  const gaspi_segment_id_t segment_id_dst = 1;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_src
					 , notification_max * sizeof(double)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_dst
					 , notification_max * sizeof(double)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  
  gaspi_pointer_t _src_ptr = NULL;
  gaspi_pointer_t _dst_ptr = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_src, &_src_ptr));
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_dst, &_dst_ptr));

  double *src = (double *) _src_ptr;
  double *dst = (double *) _dst_ptr;

#pragma omp parallel
  for (i = 0; i < notification_max; ++i)
    {
      src[i] = 1;
      dst[i] = 0;
    }

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  double time = -now();
  
  /* write notification_max doubles */
#pragma omp parallel
  for (i = 0; i < notification_max; ++i)
    {
      int tid = omp_get_thread_num();
      gaspi_queue_id_t queue_id = tid % queue_num;
      gaspi_offset_t  offset_local  = i * sizeof(double);
      gaspi_offset_t  offset_remote = i * sizeof(double);
      
      write_and_wait ( segment_id_src
		       , offset_local
		       , target
		       , segment_id_dst
		       , offset_remote
		       , sizeof(double)
		       , queue_id
		       );
    }

  /* notify in all queues */
  for (i = 0; i < queue_num; ++i)
    {
      SUCCESS_OR_DIE( gaspi_notify (segment_id_dst
				    , target
				    , (gaspi_notification_id_t) i
				    , 1
				    , (gaspi_queue_id_t) i
				    , GASPI_BLOCK
				    ));
    }

  /* wait for notify */
  for (i = 0; i < queue_num; ++i)
    {
      gaspi_notification_id_t id;
      SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id_dst
					    , (gaspi_notification_id_t) i
					    , 1
					    , &id
					    , GASPI_BLOCK
					    ));
      ASSERT(id == i);
    }
  
  time += now();
  printf("# messages sent/recveived: %d, total message rate [#/sec]: %d, bandwidth [MB/sec]: %10.6f\n"
	 , notification_max
	 , (int) ((double) 2*notification_max/time)
	 , 16*notification_max/time/1024/1024
	 ); 

  for (i = 0; i < queue_num; ++i)
    {
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id_dst
					 , (gaspi_notification_id_t) i
					 , &value
					 ));
      ASSERT(value == 1);
    }
  
  /* validate */
  for (i = 0; i < notification_max; ++i)
    {
      ASSERT(dst[i] == 1);
    }

  wait_for_flush_queues();
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

