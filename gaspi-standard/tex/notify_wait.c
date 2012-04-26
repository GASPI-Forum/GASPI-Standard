#include <GASPI.h>
#include <success_or_die.h>

gaspi_return_t
gaspi_notify_wait ( gaspi_segment_id_t segment_id
                  , gaspi_offset_t offset
                  , gaspi_timeout_t timeout
                  )
{
  gaspi_size_t size_of_flag;

  ASSERT (gaspi_notify_size (&size_of_flag));

  return gaspi_notify_waitsome ( segment_id
                               , offset
                               , size_of_flag
                               , timeout
                               );
}
