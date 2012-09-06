#include <GASPI.h>
#include <success_or_die.h>

extern void process ( const gaspi_notification_id_t id
                    , const gaspi_notification_t val
                    );

void blocking_waitsome ( const gaspi_notification_id_t id_begin
                       , const gaspi_notification_id_t id_end
                       )
{
  ASSERT ( gaspi_notify_waitsome ( id_begin
                                 , id_end - id_begin
                                 , GASPI_BLOCK
                                 )
         );

  for (gaspi_notification_id_t i = id_begin; i < id_end; ++i)
    {
      gaspi_notification_t val = 0;

      // atomic reset
      ASSERT (gaspi_notify_reset (i, &val));

      // re-check, other threads are notified too!
      if (val != 0)
        {
          process (i, val);
        }

    }
}
