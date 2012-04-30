#include <GASPI.h>
#include <success_or_die.h>

gaspi_return_t
gaspi_notify_wait ( gaspi_flag_id_t flag_id
                  , gaspi_timeout_t timeout
                  )
{

  return gaspi_notify_waitsome ( flag_id
                               , 1
                               , timeout
                               );
}
