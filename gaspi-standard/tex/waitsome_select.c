#include <GASPI.h>
#include <success_or_die.h>

extern void process (const int flag, const gaspi_flag_t val);

void waitsome ( const gaspi_segment_id_t segment_id
              , const gaspi_offset_t offset_begin
              , const int num_flags
              )
{
  gaspi_size_t size_of_flag;

  ASSERT (gaspi_notify_size (&size_of_flag));

  ASSERT ( gaspi_notify_waitsome
           ( segment_id
           , offset_begin
           , offset_begin + num_flags * size_of_flag
           , GASPI_BLOCK
           )
         );

  char* segment_ptr = 0;

  ASSERT (gaspi_segment_ptr (segment_id, &segment_ptr));

  gaspi_offset_t offset = offset_begin;
  gaspi_flag_t* flags = (gaspi_flag_t*) (segment_ptr + offset);

  for (int i = 0; i < num_flags; ++i, offset += size_of_flag)
    {
      if (flags[i] != 0)       // check for change
        {
          gaspi_flag_t val = 0;

          // atomic reset
          ASSERT (gaspi_notify_reset (segment_id, offset, &val));

          // re-check, other threads are notified too!
          if (val != 0)
            {
              process (i, val);
            }
        }
    }
}
