#include <GASPI.h>
#include <success_or_die.h>

extern void process (const int flag, const gaspi_flag_t val);

void waitsome ( const gaspi_flag_id_t flag_id_begin
              , const gaspi_flag_id_t flag_num
              )
{

  ASSERT ( gaspi_notify_waitsome
           ( flag_id_begin
           , flag_num
           , GASPI_BLOCK
           )
         );

  for (gaspi_flag_id_t i = 0; i < flag_num; ++i)
    {
      gaspi_flag_t val = 0;

      // atomic reset
      ASSERT (gaspi_notify_reset (flag_id_begin + i, &val));

      // re-check, other threads are notified too!
      if (val != 0)
        {
          process (i, val);
        }
        
    }
}
