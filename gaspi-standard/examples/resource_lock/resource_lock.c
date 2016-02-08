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

#define VAL_UNLOCKED 9999999

gaspi_return_t
global_lock_init ( const gaspi_segment_id_t seg,
		   const gaspi_offset_t off,
		   const gaspi_rank_t rank_loc,
                 , const gaspi_timeout_t timeout
                 )
{
  gaspi_rank_t iProc;
	  
  SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

  if( iProc == rank_loc)
    {
      gaspi_pointer_t vptr;
      gaspi_atomic_value_t *lock_ptr;
      
      SUCCESS_OR_RETURN(gaspi_segment_ptr, &vptr);
      lock_ptr = (gaspi_atomic_value_t *) vptr;

      *lock_ptr = VAL_UNLOCKED;
    }

  SUCCESS_OR_RETURN (gaspi_barrier ( GASPI_GROUP_ALL
				     , timeout
				     )
		     );

  return GASPI_SUCCESS;
}

gaspi_return_t
global_try_lock ( const gaspi_segment_id_t seg,
		  const gaspi_offset_t off,
		  const gaspi_rank_t rank_loc,
                , const gaspi_timeout_t timeout
                )
{
  gaspi_rank_t iProc;

  SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

  gaspi_atomic_value_t old_value;

  SUCCESS_OR_RETURN (gaspi_atomic_compare_swap ( seg
						 , off
						 , rank_loc
						 , VAL_UNLOCKED
						 , iProc
						 , &old_value
						 , timeout
                                                )
		     );

  return (old_value == VALUE_UNLOCKED) ? GASPI_SUCCESS
                                           : GASPI_ERROR
                                           ;
}

gaspi_return_t
global_unlock ( const gaspi_segment_id_t seg,
		const gaspi_offset_t off,
		const gaspi_rank_t rank_loc,
              , const gaspi_timeout_t timeout
              )
{
  gaspi_rank_t iProc;

  SUCCESS_OR_RETURN (gaspi_proc_rank (&iProc));

  gaspi_atomic_value_t current_value;

  SUCCESS_OR_RETURN (gaspi_atomic_compare_swap ( seg
						 , off
						 , rank_loc
                                                 , iProc
						 , VAL_UNLOCKED
						 , &current_value
						 , timeout
                                                )
                    );

  return GASPI_SUCCESS;
}
