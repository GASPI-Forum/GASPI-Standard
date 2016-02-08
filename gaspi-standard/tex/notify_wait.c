#include <GASPI.h>
#include <success_or_die.h>

gaspi_return_t
gaspi_notify_wait ( const gaspi_notification_id_t notify_id
                  , const gaspi_timeout_t timeout
		  , const gaspi_segment_id_t seg_id  
                  )
{

  gaspi_notification_id_t first_id;

  return gaspi_notify_waitsome ( seg_id,
			       , notify_id
                               , 1
			       , &first_id,
                               , timeout
                               );
}

