#include <GASPI.h>
#include <success_or_die.h>

extern void process ( const gaspi_notification_id_t id
                    , const gaspi_notification_t val );

void waitsome ( const gaspi_notification_id_t id_begin
              , const gaspi_notification_id_t id_num
              )
{

  ASSERT ( gaspi_notify_waitsome
           ( id_begin
           , id_num
           , GASPI_BLOCK
           )
         );

  for (gaspi_notification_id_t i = 0; i < id_num; ++i)
    {
      gaspi_notification_t notification = 0;

      // atomic reset
      ASSERT (gaspi_notify_reset (id_begin + i, &notification));

      // re-check, other threads are notified too!
      if (notification != 0)
        {
          process (i, notification);
        }
        
    }
}
