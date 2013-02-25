#include "gaspi_gpi.h"
#include <GPI.h>
#include <GpiLogger.h>
#include <assert.h>
#include <stdarg.h>

#define CHECK(x) ( (x)!=0 ? GASPI_ERROR : GASPI_SUCCESS )

#pragma weak gaspi_proc_init    = pgaspi_proc_init
#pragma weak gaspi_proc_term    = pgaspi_proc_term
#pragma weak gaspi_barrier      = pgaspi_barrier
#pragma weak gaspi_proc_rank    = pgaspi_proc_rank
#pragma weak gaspi_proc_num     = pgaspi_proc_num
#pragma weak gaspi_wait         = pgaspi_wait
#pragma weak gaspi_write        = pgaspi_write 
#pragma weak gaspi_read         = pgaspi_read
#pragma weak gaspi_write_notify = pgaspi_write_notify
#pragma weak gaspi_notify       = pgaspi_notify
#pragma weak gaspi_notify_waitsome  = pgaspi_notify_waitsome
#pragma weak gaspi_notify_reset     = pgaspi_notify_reset
#pragma weak gaspi_notification_num = pgaspi_notification_num
#pragma weak gaspi_segment_ptr      = pgaspi_segment_ptr
#pragma weak gaspi_allreduce        = pgaspi_allreduce


gaspi_notification_id_t notify_num_max = 0;
gaspi_offset_t notify_buffer_offset = 0;
gaspi_notification_t* notify_buffer_pointer = NULL;
gaspi_size_t gaspi_notify_value_size = sizeof(gaspi_notification_t);

void
gaspi_printf( char * format , ... )
{
  va_list arglist;
  va_start(arglist, format);
  gpi_vprintf( format, arglist );
  va_end(arglist);
}

//AllReduce: maps gaspi data types to gpi data types 
gaspi_return_t
to_gpi_data_types(gaspi_datatype_t gaspi_type, GPI_TYPE* gpi_type, gaspi_size_t* data_size)
{
	switch (gaspi_type)
	{
		case GASPI_TYPE_INT: *gpi_type = GPI_INT; *data_size = sizeof(int); break;
		case GASPI_TYPE_UINT: *gpi_type = GPI_UINT; *data_size = sizeof(unsigned int); break;
		case GASPI_TYPE_LONG: *gpi_type = GPI_LONG; *data_size = sizeof(long); break;
		case GASPI_TYPE_ULONG: *gpi_type = GPI_ULONG; *data_size = sizeof(unsigned long); break;
		case GASPI_TYPE_FLOAT: *gpi_type = GPI_FLOAT; *data_size = sizeof(float); break;
		case GASPI_TYPE_DOUBLE: *gpi_type = GPI_DOUBLE; *data_size = sizeof(double); break;
		default: return GASPI_ERROR;
	}
	
	return GASPI_SUCCESS;
}

//AllReduce: maps gaspi operation to gpi operation 
GPI_OP 
to_gpi_operation(gaspi_operation_t gaspi_operation)
{
	switch (gaspi_operation)
	{
		case GASPI_OP_MIN: return GPI_MIN;
		case GASPI_OP_MAX: return GPI_MAX;
		case GASPI_OP_SUM: return GPI_SUM;
		default: return GPI_MIN;
	}
}

gaspi_return_t
pgaspi_proc_init ( gaspi_configuration_t configuration
                , gaspi_timeout_t timeout
                )
{
	assert(timeout == GASPI_BLOCK);
	
	argument_t *arg = (argument_t *) configuration.user_defined;
	notify_num_max = configuration.notify_flag_num;
	notify_buffer_offset = (1UL << 30);
	
	int status = startGPI(arg->argc, arg->argv, 0, notify_buffer_offset + (notify_num_max + gaspi_notify_value_size));
	if(CHECK(status) == GASPI_ERROR)
		return GASPI_ERROR;
	
	void* temp;		
	pgaspi_segment_ptr(0, &temp);
	notify_buffer_pointer = (gaspi_notification_t*) (temp + notify_buffer_offset);
	
	return GASPI_SUCCESS;
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

gaspi_return_t
pgaspi_barrier ( gaspi_group_t group
              , gaspi_timeout_t timeout
              )
{
	assert(group == GASPI_GROUP_ALL);
	assert(timeout == GASPI_BLOCK);
	
	barrierGPI();
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_wait (gaspi_queue_id_t queue
           , gaspi_timeout_t timeout)
{
	assert(timeout == GASPI_BLOCK);
	
	return CHECK(waitDmaGPI(queue));
}

gaspi_return_t
pgaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_id_t queue
            , gaspi_timeout_t timeout
            )
{
	assert(segment_id_local == 0);
	assert(segment_id_remote == 0);
	assert(timeout == GASPI_BLOCK);
	assert((offset_local + size) < notify_buffer_offset);
	assert((offset_remote + size) < notify_buffer_offset);

	return CHECK(writeDmaGPI(offset_local, offset_remote, size, rank, queue));
}

gaspi_return_t
pgaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           )
{
	assert(segment_id_local == 0);
	assert(segment_id_remote == 0);
	assert(timeout == GASPI_BLOCK);
	assert((offset_local + size) < notify_buffer_offset);
	assert((offset_remote + size) < notify_buffer_offset);
	
	return CHECK(readDmaGPI (offset_local, offset_remote, size, rank, queue));
}

gaspi_return_t
pgaspi_write_notify( gaspi_segment_id_t segment_id_local
                   , gaspi_offset_t offset_local
                   , gaspi_rank_t rank
                   , gaspi_segment_id_t segment_id_remote
                   , gaspi_offset_t offset_remote
                   , gaspi_size_t size
                   , gaspi_notification_id_t flag_id
                   , gaspi_notification_t flag_value
                   , gaspi_queue_id_t queue
                   , gaspi_timeout_t timeout
                   )
{
	assert(segment_id_local == 0);
	assert(segment_id_remote == 0);
	assert(timeout == GASPI_BLOCK);
	assert((offset_local + size) < notify_buffer_offset);
	assert((offset_remote + size) < notify_buffer_offset);
	assert(flag_id < notify_num_max);

	if(pgaspi_write(segment_id_local, offset_local, rank, segment_id_remote, offset_remote, size, queue, timeout) == GASPI_ERROR)
		return GASPI_ERROR;
	
	return CHECK(pgaspi_notify(rank, flag_id, flag_value, queue, timeout));
}


gaspi_return_t
pgaspi_notify ( gaspi_rank_t rank
             , gaspi_notification_id_t flag_id
             , gaspi_notification_t flag_value
             , gaspi_queue_id_t queue
             , gaspi_timeout_t timeout
             )
{
	assert(timeout == GASPI_BLOCK);
	assert(notify_buffer_pointer != NULL);
	assert(flag_id < notify_num_max);
	
	notify_buffer_pointer[flag_id] = flag_value;
	gaspi_offset_t offset = notify_buffer_offset + flag_id * gaspi_notify_value_size;
	
	return CHECK(writeDmaGPI(offset, offset, gaspi_notify_value_size, rank, queue));
}

gaspi_return_t
pgaspi_notify_waitsome ( gaspi_notification_id_t flag_id_beg
                      , gaspi_notification_id_t flag_num
                      , gaspi_timeout_t timeout
                      )
{
	assert(timeout == GASPI_BLOCK);
	gaspi_notification_id_t flag_id_end = flag_id_beg + flag_num - 1;
	assert(flag_id_end < notify_num_max);

	volatile gaspi_notification_t* flag_value = (gaspi_notification_t*) notify_buffer_pointer;
	gaspi_notification_id_t i;
	char notify_found = 0;
	while(!notify_found)
	{
		for(i = flag_id_beg; i < flag_num; i++)
		{	
			if(flag_value[i] != 0)
			{	
				notify_found = 1;
				break;	
			}
		}
	}
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_notify_reset ( gaspi_notification_id_t flag_id
                   , gaspi_notification_t* flag_val
                   )
{
	assert(notify_buffer_offset > 0);
	assert(flag_id < notify_num_max);
	
	*flag_val = notify_buffer_pointer[flag_id];
	notify_buffer_pointer[flag_id] = 0;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_notification_num ( gaspi_notification_id_t* flag_max )
{
	*flag_max = notify_num_max;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_segment_ptr ( gaspi_segment_id_t segment_id
                  , gaspi_pointer_t* pointer
                  )
{
	assert(segment_id == 0);
	
	*pointer = (gaspi_pointer_t) getDmaMemPtrGPI();
	
	return GASPI_SUCCESS;
}

gaspi_return_t pgaspi_allreduce(gaspi_pointer_t buffer_send
           , gaspi_pointer_t buffer_receive
           , unsigned char num
           , gaspi_operation_t operation
           , gaspi_datatype_t datatype
           , gaspi_group_t group
           , gaspi_timeout_t timeout
           )
{
	assert(group == GASPI_GROUP_ALL);
	assert(timeout == GASPI_BLOCK);
	
	gaspi_size_t data_size;
	GPI_TYPE data_type_gpi;
	if(to_gpi_data_types(datatype, &data_type_gpi, &data_size) == GASPI_ERROR)
		return GASPI_ERROR;
		
	assert((buffer_send + (data_size * num)) < (void*) notify_buffer_pointer);
	assert((buffer_receive + (data_size * num)) < (void*) notify_buffer_pointer);
	
	return CHECK(allReduceGPI(buffer_send, buffer_receive, num, to_gpi_operation(operation), data_type_gpi));
}


/*

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

// not implemented in gpi
gaspi_return_t
gaspi_proc_kill ( gaspi_rank_t rank
                , gaspi_timeout_t timeout )
{
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

