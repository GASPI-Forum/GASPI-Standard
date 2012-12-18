#include "gaspi_gpi.h"
#include <GPI.h>
#include <assert.h>


#pragma weak gaspi_proc_init = pgaspi_proc_init
#pragma weak gaspi_proc_term = pgaspi_proc_term
#pragma weak gaspi_proc_rank = pgaspi_proc_rank
#pragma weak gaspi_proc_num = pgaspi_proc_num

gaspi_return_t
pgaspi_proc_init ( gaspi_configuration_t configuration
                , gaspi_timeout_t timeout
                )
{
  assert(timeout == GASPI_BLOCK);
  argument_t *arg = (argument_t *) configuration.user_defined;
  return startGPI(arg->argc, arg->argv, "", (1UL << 30)) != 0 ? GASPI_ERROR : GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_proc_term (gaspi_timeout_t timeout )
{
  assert(timeout == GASPI_BLOCK);
  shutdownGPI();
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_proc_rank (gaspi_rank_t *rank)
{
	*rank = getRankGPI();
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_proc_num ( gaspi_rank_t *proc_num )
{
  *proc_num = getNodeCountGPI();
	return GASPI_SUCCESS;
}
/*




gaspi_return_t
gaspi_proc_kill ( gaspi_rank_t rank
                , gaspi_timeout_t timeout )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_proc_rank (gaspi_rank_t* rank)
{
  *rank = getRankGPI ();
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_proc_num ( gaspi_rank_t* proc_num )
{
  *proc_num = getNodeCountGPI ();
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_state_vec_get( gaspi_state_vector_t state_vector )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_state_vec_reset( gaspi_state_vector_t state_vector )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_create (gaspi_group_t group)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_add ( gaspi_group_t group
                , gaspi_rank_t rank
                )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_commit ( gaspi_group_t group
                   , gaspi_timeout_t timeout
                   )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_delete ( gaspi_group_t group )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_max (gaspi_number_t group_max)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_size ( gaspi_group_t group
                 , gaspi_rank_t group_size )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_group_ranks( gaspi_group_t group
		   , gaspi_rank_t  group_ranks
		   , gaspi_number_t group_size)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_alloc ( gaspi_segment_id_t segment_id
                    , gaspi_size_t size
                    , gaspi_number_t alloc_policy
                    )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_register ( gaspi_segment_id_t segment_id
                       , gaspi_rank_t rank
                       , gaspi_timeout_t timeout
                       )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_create ( gaspi_segment_id_t segment_id
                     , gaspi_size_t size
                     , gaspi_group_t group
                     , gaspi_timeout_t timeout
                     , gaspi_number_t alloc_policy
                     )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_free ( gaspi_segment_id_t segment_id )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_ptr ( gaspi_segment_id_t segment_id
                  , gaspi_pointer_t* pointer
                  )
{
  assert(segment_id == 0);
  *pointer = getDmaMemPtrGPI ();
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_segment_max (gaspi_segment_id_t segment_max)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_t queue
            , gaspi_timeout_t timeout
            )
{
  assert(segment_id_local == 0);
  assert(segment_id_remote == 0);
  assert(timeout == GASPI_BLOCK);

  writeDmaGPI (offset_local, offset_remote, size, rank, queue);
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_t queue
           , gaspi_timeout_t timeout
           )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_wait ( gaspi_queue_t queue
           , gaspi_timeout_t timeout
           )
{
  assert(timeout == GASPI_BLOCK);
  waitDmaGPI (queue);
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_notify ( gaspi_rank_t rank
             , gaspi_flag_t flag
             , gaspi_segment_id_t segment_id_flag
             , gaspi_offset_t offset_flag
             , gaspi_queue_t queue
             , gaspi_timeout_t timeout
             )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_notify_wait ( gaspi_flag_t comparator
                  , gaspi_segment_id_t segment_id
                  , gaspi_offset_t offset
                  , gaspi_timeout_t timeout
                  )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_notify_reset ( gaspi_flag_t flag_val
                   , gaspi_segment_id_t segment_id
                   , gaspi_offset_t offset
                   )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_notify_size ( gaspi_size_t size_flag )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_write_notify ( gaspi_segment_id_t segment_id_local
                   , gaspi_offset_t offset_local
                   , gaspi_rank_t rank
                   , gaspi_segment_id_t segment_id_remote
                   , gaspi_offset_t offset_remote
                   , gaspi_size_t size
                   , gaspi_flag_t flag
                   , gaspi_segment_id_t segment_id_flag
                   , gaspi_offset_t offset_flag
                   , gaspi_queue_t queue
                   , gaspi_timeout_t timeout
                   )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_write_list ( gaspi_number_t num
                 , gaspi_segment_id_t segment_id_local[num]
                 , gaspi_offset_t offset_local[num]
                 , gaspi_rank_t rank
                 , gaspi_segment_id_t segment_id_remote[num]
                 , gaspi_offset_t offset_remote[num]
                 , gaspi_size_t size[num]
                 , gaspi_queue_t queue
                 , gaspi_timeout_t timeout
                 )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_write_list_notify ( gaspi_number_t num
                        , gaspi_segment_id_t segment_id_local[num]
                        , gaspi_offset_t offset_local[num]
                        , gaspi_rank_t rank
                        , gaspi_segment_id_t segment_id_remote[num]
                        , gaspi_offset_t offset_remote[num]
                        , gaspi_size_t size[num]
                        , gaspi_flag_t flag
                        , gaspi_segment_id_t segment_id_flag
                        , gaspi_offset_t offset_flag
                        , gaspi_queue_t queue
                        , gaspi_timeout_t timeout
                        )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_read_list ( gaspi_number_t num
                , gaspi_segment_id_t segment_id_local[num]
                , gaspi_offset_t offset_local[num]
                , gaspi_rank_t rank
                , gaspi_segment_id_t segment_id_remote[num]
                , gaspi_offset_t offset_remote[num]
                , gaspi_size_t size[num]
                , gaspi_queue_t queue
                , gaspi_timeout_t timeout
                )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_queue_num (gaspi_queue_t queue_max )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_queue_size ( gaspi_queue_t queue
                 , gaspi_number_t queue_size
                 )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_queue_size_max ( gaspi_queue_t queue
                     , gaspi_number_t queue_size
                     )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_messg_size_max ( gaspi_size_t messg_size )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_send ( gaspi_segment_id_t segment_id_local
                   , gaspi_offset_t offset_local
                   , gaspi_rank_t rank
                   , gaspi_size_t size
                   , gaspi_tag_t tag
                   , gaspi_timeout_t timeout
                   )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_wait (gaspi_timeout_t timeout)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_receive ( gaspi_segment_id_t segment_id_local
                      , gaspi_offset_t offset_local
                      , gaspi_rank_t rank
                      , gaspi_tag_t tag
                      , gaspi_timeout_t timeout
                      )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_queue_size (gaspi_number_t queue_size)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_queue_size_max  (gaspi_number_t queue_size)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_passive_messg_size_max (gaspi_size_t messg_size)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_counter_reset ( gaspi_counter_id_t counter_id
                    , gaspi_counter_value_t value
                    , gaspi_timeout_t timeout
                    )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_counter_fetch_add ( gaspi_counter_id_t counter_id
                        , gaspi_counter_value_t value_add
                        , gaspi_counter_value_t value_old
                        , gaspi_timeout_t timeout
                        )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_counter_compare_swap ( gaspi_counter_id_t counter_id
                           , gaspi_counter_value_t comparator
                           , gaspi_counter_value_t value_new
                           , gaspi_counter_value_t value_old
                           , gaspi_timeout_t timeout
                           )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_counter_num (gaspi_number_t counter_max)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_barrier ( gaspi_group_t group
              , gaspi_timeout_t timeout
              )
{
  assert(group == GASPI_GROUP_ALL);
  assert(timeout == GASPI_BLOCK);
  barrierGPI ();
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_allreduce ( gaspi_pointer_t buffer_send
                , gaspi_pointer_t buffer_receive
                , unsigned char num
                , gaspi_operation_t operation
                , gaspi_datatype_t datatype
                , gaspi_group_t group
                , gaspi_timeout_t timeout
                )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_allreduce_elem_max (gaspi_number_t elem_max)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_allreduce_user ( gaspi_pointer_t buffer_send
                     , gaspi_pointer_t buffer_receive
                     , unsigned char num
                     , gaspi_size_t size_element
                     , gaspi_reduce_operation_t reduce_operation
                     , gaspi_reduce_state_t reduce_state
                     , gaspi_group_t group
                     , gaspi_timeout_t timeout
                     )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_reduce_operation ( gaspi_pointer_t operand_one
                       , gaspi_pointer_t operand_two
                       , gaspi_pointer_t result
                       , gaspi_state_t state
                       , gaspi_timeout_t timeout
                       )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_allreduce_buf_size (gaspi_number_t buf_size)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_version (float version)
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_time_get ( gaspi_time_t wtime )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_time_ticks ( gaspi_time_t resolution )
{
	return GASPI_SUCCESS;
}





gaspi_return_t
gaspi_error_messg ( gaspi_return_t error_code
                  , gaspi_string_t error_messg )
{
	return GASPI_SUCCESS;
}
*/

