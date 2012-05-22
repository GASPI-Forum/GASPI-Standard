#include <GASPI.h>
#include <success_or_die.h>

gaspi_return_t
gaspi_notify_wait ( gaspi_notification_id_t notify_id
                  , gaspi_timeout_t timeout
                  )
{

  return gaspi_notify_waitsome ( notify_id
                               , 1
                               , timeout
                               );
}
