#include <GASPI.h>
#include <success_or_die.h>

extern void process ( const gaspi_notification_id_t id
                    , const gaspi_notification_t val
                    );

void blocking_waitsome ( const gaspi_notification_id_t id_begin
                       , const gaspi_notification_id_t id_end
                       , const gaspi_segment_id_t seg_id
                       )
{
  gaspi_notification_id_t first_id;
  
  ASSERT ( gaspi_notify_waitsome ( seg_id
				 , id_begin
                                 , id_end - id_begin
                                 , &first_id
                                 , GASPI_BLOCK
                                 )
         );

  gaspi_notification_t val = 0;

  // atomic reset
  ASSERT (gaspi_notify_reset (seg_id, first_id, &val));

  // other threads are notified too!
  process (first_id, val);
}
