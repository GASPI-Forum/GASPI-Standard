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
#pragma weak gaspi_passive_send     = pgaspi_passive_send
#pragma weak gaspi_passive_wait     = pgaspi_passive_wait
#pragma weak gaspi_passive_receive  = pgaspi_passive_receive
#pragma weak gaspi_segment_ptr      = pgaspi_segment_ptr
#pragma weak gaspi_allreduce        = pgaspi_allreduce
#pragma weak gaspi_queue_size      = pgaspi_queue_size
#pragma weak gaspi_statistic_verbosity_level = pgaspi_statistic_verbosity_level
#pragma weak gaspi_statistic_counter_max = pgaspi_statistic_counter_max
#pragma weak gaspi_statistic_counter_info = pgaspi_statistic_counter_info
#pragma weak gaspi_statistic_counter_get = pgaspi_statistic_counter_get
#pragma weak gaspi_statistic_counter_reset = pgaspi_statistic_counter_reset


gaspi_notification_id_t notify_num_max = 0;
gaspi_offset_t notify_buffer_offset = 0;
gaspi_notification_t* notify_buffer_pointer = NULL;
gaspi_size_t gaspi_notify_value_size = sizeof(gaspi_notification_t);

gaspi_number_t verbosity_level = 0;

gaspi_rank_t number_ranks = 0;

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
	notify_buffer_offset = (1UL << 20);
	
	int status = startGPI(arg->argc, arg->argv, 0, notify_buffer_offset + (2 * notify_num_max * gaspi_notify_value_size));
	if(CHECK(status) == GASPI_ERROR)
		return GASPI_ERROR;
	
	void* temp;		
	pgaspi_segment_ptr(0, &temp);
	notify_buffer_pointer = (gaspi_notification_t*) ((char*) temp + notify_buffer_offset);

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
	
	notify_buffer_pointer[notify_num_max + flag_id] = flag_value;
	gaspi_offset_t offset = notify_buffer_offset + flag_id * gaspi_notify_value_size;
	
	return CHECK(writeDmaGPI(offset + (notify_num_max * gaspi_notify_value_size), offset, gaspi_notify_value_size, rank, queue));
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
		for(i = flag_id_beg; i <= flag_id_end; i++)
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
	volatile gaspi_notification_t temp_value = notify_buffer_pointer[flag_id];
	*flag_val = temp_value;
	notify_buffer_pointer[flag_id] = 0;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_notification_num ( gaspi_notification_id_t* flag_max )
{
	*flag_max = notify_num_max;
	
	return GASPI_SUCCESS;
}

extern gaspi_return_t pgaspi_passive_send ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_size_t size
           , gaspi_tag_t tag
           , gaspi_timeout_t timeout
           )
{
	assert(segment_id_local == 0);
	assert(timeout == GASPI_BLOCK);
	assert(tag == 0);
	
	return CHECK(sendDmaPassiveGPI(offset_local, size, rank));
}

extern gaspi_return_t pgaspi_passive_wait (gaspi_timeout_t timeout)
{
	assert(timeout == GASPI_BLOCK);
	
	waitDmaPassiveGPI();
	
	return GASPI_SUCCESS;	
}

extern gaspi_return_t pgaspi_passive_receive ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_size_t size
           , gaspi_rank_t* rank
           , gaspi_tag_t* tag
           , gaspi_timeout_t timeout
           )
{
	assert(segment_id_local == 0);
	assert(timeout == GASPI_BLOCK);
	
	int senderRank = -1;
	if(CHECK(recvDmaPassiveGPI(offset_local, size, &senderRank)) == GASPI_ERROR)
		return GASPI_ERROR;
	
	*rank = senderRank;
	
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
		
	assert((void*)((char*) buffer_send + (data_size * num)) < notify_buffer_pointer);
	assert((void*)((char*) buffer_receive + (data_size * num)) < notify_buffer_pointer);
	
	return CHECK(allReduceGPI(buffer_send, buffer_receive, num, to_gpi_operation(operation), data_type_gpi));
}

gaspi_return_t
pgaspi_queue_size(gaspi_queue_id_t queue, gaspi_number_t* queue_size)
{
	int temp = openDMARequestsGPI(queue);
	if(temp < 0)
		return GASPI_ERROR;
		
	*queue_size = (gaspi_number_t) temp;

	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_statistic_verbosity_level(gaspi_number_t _verbosity_level)
{
	verbosity_level = _verbosity_level;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_statistic_counter_max(gaspi_statistic_counter_t* counter_max)
{
	*counter_max = 0;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_statistic_counter_info(gaspi_statistic_counter_t counter
			, gaspi_statistic_argument_t* counter_argument
			, gaspi_string_t* counter_name
			, gaspi_string_t* counter_description
			, gaspi_number_t* verbosity_level
			)
{
	return GASPI_SUCCESS;
}

gaspi_return_t
pgaspi_statistic_counter_get ( gaspi_statistic_counter_t counter
			, gaspi_number_t argument
			, gaspi_number_t value
			)
{
	return GASPI_SUCCESS;
}
			
gaspi_return_t
pgaspi_statistic_counter_reset (gaspi_statistic_counter_t counter)
{
	return GASPI_SUCCESS;
}
