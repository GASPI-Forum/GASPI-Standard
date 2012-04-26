#include <GASPI.h>
#include <assert.h>

#define SUCCESS_OR_RETURN(f)                    \
  {                                             \
    const int ec = (f);                         \
                                                \
    if (ec != GASPI_SUCCESS)                    \
      {                                         \
        return ec;                              \
      }                                         \
  }

#define VAL_UNLOCKED -1
#define VAL_NONE -2

gaspi_return_t
global_lock_init ( const gaspi_counter_id_t lock_cnt_id
                 , const gaspi_timeout_t timeout
                 )
{
  static unsigned short phase = 0;

  gaspi_timeout_t time_left = timeout;

  switch (phase)
    {
    case 0:
      {
        gaspi_rank_t iProc = GASPI_NORANK;

        SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

        if (0 == iProc)
          {
            gaspi_time_t time_start = GASPI_NOTIME;
            gaspi_time_t time_end = GASPI_NOTIME;

            SUCCESS_OR_RETURN (gaspi_time_get (&time_start));
            SUCCESS_OR_RETURN (gaspi_counter_reset ( lock_cnt_id
                                                   , VAL_UNLOCKED
                                                   , time_left
                                                   )
                              );
            SUCCESS_OR_RETURN (gaspi_time_get (&time_end));

            if (timeout >= 0)
              {
                time_left -= time_end - time_start;
              }
          }

        phase = 1;
      }
    case 1:
      if (timeout >= 0 && time_left < 0)
        {
          return GASPI_TIMEOUT;
        }

      SUCCESS_OR_RETURN (gaspi_barrier ( GASPI_GROUP_ALL
                                       , time_left
                                       )
                        );

      phase = 0;

    default:
      assert (false);
    }

  return GASPI_SUCCESS;
}

gaspi_return_t
global_try_lock ( const gaspi_counter_id_t lock_cnt_id
                , const gaspi_timeout_t timeout
                )
{
  gaspi_rank_t iProc = GASPI_NORANK;

  SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

  gaspi_counter_value_t current_value = VAL_NONE;

  SUCCESS_OR_RETURN (gaspi_counter_compare_swap ( lock_cnt_id
                                                , VALUE_UNLOCKED
                                                , iProc
                                                , &current_value
                                                , timeout
                                                )
                    );

  return (current_value == VALUE_UNLOCKED) ? GASPI_SUCCESS
                                           : GASPI_ERROR
                                           ;
}

gaspi_return_t
global_unlock ( const gaspi_counter_id_t lock_cnt_id
              , const gaspi_timeout_t timeout
              )
{
  gaspi_rank_t iProc = GASPI_NORANK;

  SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

  gaspi_counter_value_t current_value = VAL_NONE;

  SUCCESS_OR_RETURN (gaspi_counter_compare_swap ( lock_cnt_id
                                                , iProc
                                                , VALUE_UNLOCKED
                                                , &current_value
                                                , timeout
                                                )
                    );

  assert (current_value == iProc);

  return GASPI_SUCCESS;
}
