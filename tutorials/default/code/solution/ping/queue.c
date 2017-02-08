#include "queue.h"
#include "success_or_die.h"
#include "assert.h"


static int my_queue = 0;
#pragma omp threadprivate(my_queue)

void
notify_and_wait ( gaspi_segment_id_t const segment_id_remote
	        , gaspi_rank_t const rank
		, gaspi_notification_id_t const notification_id
	        , gaspi_notification_t const notification_value
	        , gaspi_queue_id_t const queue
	        )
{
  gaspi_timeout_t const timeout = GASPI_BLOCK;
  gaspi_return_t ret;

  /* notify, wait if required and re-submit */
  while ((ret = ( gaspi_notify (segment_id_remote, rank, 
				notification_id, notification_value, 
				queue, timeout)
		  )) == GASPI_QUEUE_FULL)
    {
      SUCCESS_OR_DIE (gaspi_wait (queue,
				  GASPI_BLOCK));
    }
  ASSERT (ret == GASPI_SUCCESS);  
}

void
write_and_wait ( gaspi_segment_id_t const segment_id_local
	       , gaspi_offset_t const offset_local
	       , gaspi_rank_t const rank
	       , gaspi_segment_id_t const segment_id_remote
	       , gaspi_offset_t const offset_remote
	       , gaspi_size_t const size
	       , gaspi_queue_id_t const queue
	       )
{
  gaspi_timeout_t const timeout = GASPI_BLOCK;
  gaspi_return_t ret;
  
  /* write, wait if required and re-submit */
  while ((ret = ( gaspi_write( segment_id_local, offset_local, rank,
			       segment_id_remote, offset_remote, size,
			       queue, timeout)
	    )) == GASPI_QUEUE_FULL)
    {
      SUCCESS_OR_DIE (gaspi_wait (queue,
				  GASPI_BLOCK));
    }
  ASSERT (ret == GASPI_SUCCESS);
}

void
write_notify_and_cycle ( gaspi_segment_id_t const segment_id_local
		       , gaspi_offset_t const offset_local
		       , gaspi_rank_t const rank
		       , gaspi_segment_id_t const segment_id_remote
		       , gaspi_offset_t const offset_remote
		       , gaspi_size_t const size
		       , gaspi_notification_id_t const notification_id
		       , gaspi_notification_t const notification_value
		       )
{
  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));

  gaspi_timeout_t const timeout = GASPI_BLOCK;
  gaspi_return_t ret;

  
  /* write, cycle if required and re-submit */
  while ((ret = ( gaspi_write_notify( segment_id_local, offset_local, rank,
				      segment_id_remote, offset_remote, size,
				      notification_id, notification_value,
				      my_queue, timeout)
		  )) == GASPI_QUEUE_FULL)
    {
      my_queue = (my_queue + 1) % queue_num;
      SUCCESS_OR_DIE (gaspi_wait (my_queue,
				  GASPI_BLOCK));
    }
  ASSERT (ret == GASPI_SUCCESS);
}

void
wait_for_flush_queues()
{
  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));

  gaspi_queue_id_t queue = 0;
  
  /* cycle all queues and wait */
  while (queue < queue_num)
    {
      SUCCESS_OR_DIE(gaspi_wait ( queue,
				  GASPI_BLOCK));
      ++queue;
    }
}
