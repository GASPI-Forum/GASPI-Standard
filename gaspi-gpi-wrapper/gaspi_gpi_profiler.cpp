#include "gaspi_gpi_profiler.h"
#include <iostream>
#include <assert.h>
#include <string.h>

Counter* counter[COUNTER_NUMBER_MAX];
gaspi_number_t verbosity_level = 0;

OperationAdd* op_add = new OperationAdd();
OperationMin* op_min = new OperationMin();
OperationMax* op_max = new OperationMax();

gaspi_return_t
gaspi_proc_init ( gaspi_configuration_t configuration
                , gaspi_timeout_t timeout
                )
{	
	counter[COUNTER_FUNC_INVOC] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_INVOC", "Measures the the invocation of functions", 1, op_add);
	counter[COUNTER_FUNC_BYTES_TOTAL] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_BYTES_TOTAL", "Measures the transferred bytes for different functions", 1, op_add);
	counter[COUNTER_FUNC_BYTES_MIN] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_BYTES_MIN", "Stores the minimum amount of transferred bytes for different functions", 2, op_min);
	counter[COUNTER_FUNC_BYTES_MAX] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_BYTES_MAX", "Stores the maximum amount of transferred bytes for different functions", 2, op_max);
	counter[COUNTER_FUNC_TIME_TOTAL] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_TIME_VALUE", "Measures the runtime time of functions.", 1, op_add);
	counter[COUNTER_FUNC_TIME_MIN] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_TIME_MIN", "Stores the shortest runtime of functions.", 2, op_min);
	counter[COUNTER_FUNC_TIME_MAX] = new CounterBasicFunc(GASPI_STATISTIC_ARGUMENT_FUNC, "COUNTER_FUNC_TIME_MAX", "Stores the longest runtime of functions.", 2, op_max);
	counter[COUNTER_RANK_WRITE_VALUE] = new CounterBasicRank(GASPI_STATISTIC_ARGUMENT_RANK, "COUNTER_RANK_WRITE_VALUE", "Counts all written Bytes", 3, op_add);
	counter[COUNTER_RANK_READ_VALUE] = new CounterBasicRank(GASPI_STATISTIC_ARGUMENT_RANK, "COUNTER_RANK_READ_VALUE", "Counts all written Bytes", 3, op_add);
	
	
	gaspi_return_t status = pgaspi_proc_init(configuration, timeout);
	
	return status;
}

gaspi_return_t
gaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_id_t queue
            , gaspi_timeout_t timeout
            )
{
	gaspi_time_t start, end, diff;
	pgaspi_time_get(&start);
	
	gaspi_return_t status = pgaspi_write(segment_id_local, offset_local, 
		rank, segment_id_remote, offset_remote, size, queue, timeout);
		
	pgaspi_time_get(&end);
	diff = end - start;
 
	if(status == GASPI_SUCCESS && verbosity_level > 0)
	{
		(*counter[COUNTER_FUNC_INVOC]).doOperation(FUNC_GASPI_WRITE, 1);
		(*counter[COUNTER_FUNC_BYTES_TOTAL]).doOperation(FUNC_GASPI_WRITE, size);
		(*counter[COUNTER_FUNC_TIME_TOTAL]).doOperation(FUNC_GASPI_WRITE, diff);
		if(verbosity_level > 1)
		{
			
			(*counter[COUNTER_FUNC_BYTES_MIN]).doOperation(FUNC_GASPI_WRITE, size);
			(*counter[COUNTER_FUNC_BYTES_MAX]).doOperation(FUNC_GASPI_WRITE, size);
			(*counter[COUNTER_FUNC_TIME_MIN]).doOperation(FUNC_GASPI_WRITE, diff);
			(*counter[COUNTER_FUNC_TIME_MAX]).doOperation(FUNC_GASPI_WRITE, diff);
			if(verbosity_level > 2)
			{
				(*counter[COUNTER_RANK_WRITE_VALUE]).doOperation(rank, size);
			}
		}	
	}
		
	return status;
}

gaspi_return_t
gaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           )
{
	gaspi_time_t start, end, diff;
	pgaspi_time_get(&start);
	
	gaspi_return_t status = pgaspi_read(segment_id_local, offset_local, 
		rank, segment_id_remote, offset_remote, size, queue, timeout);
	
	pgaspi_time_get(&end);
	diff = end - start;
	
	if(status == GASPI_SUCCESS && verbosity_level > 0)
	{
		(*counter[COUNTER_FUNC_INVOC]).doOperation(FUNC_GASPI_READ, 1);
		(*counter[COUNTER_FUNC_BYTES_TOTAL]).doOperation(FUNC_GASPI_READ, size);
		(*counter[COUNTER_FUNC_TIME_TOTAL]).doOperation(FUNC_GASPI_READ, diff);
		if(verbosity_level > 1)
		{
			(*counter[COUNTER_FUNC_BYTES_MIN]).doOperation(FUNC_GASPI_READ, size);
			(*counter[COUNTER_FUNC_BYTES_MAX]).doOperation(FUNC_GASPI_READ, size);
			(*counter[COUNTER_FUNC_TIME_MIN]).doOperation(FUNC_GASPI_READ, diff);
			(*counter[COUNTER_FUNC_TIME_MAX]).doOperation(FUNC_GASPI_READ, diff);
			if(verbosity_level > 2)
			{
				(*counter[COUNTER_RANK_READ_VALUE]).doOperation(rank, size);
			}
		}	
	}	
	
	return status;
}

gaspi_return_t
gaspi_statistic_verbosity_level(gaspi_number_t _verbosity_level)
{
	verbosity_level = _verbosity_level;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
gaspi_statistic_counter_max(gaspi_statistic_counter_t* counter_max)
{
	*counter_max = COUNTER_NUMBER_MAX;
	
	return GASPI_SUCCESS;
}

gaspi_return_t
gaspi_statistic_counter_info(gaspi_statistic_counter_t counter_id
			, gaspi_statistic_argument_t* counter_argument
			, gaspi_string_t* counter_name
			, gaspi_string_t* counter_description
			, gaspi_number_t* verbosity_level
			)
{
	assert(counter_id < COUNTER_NUMBER_MAX);
	
	*counter_argument = (*counter[counter_id]).getStatisticArgument();
	std::string name = (*counter[counter_id]).getName();
	std::string description = (*counter[counter_id]).getDescription();
	strncpy(*counter_name, name.c_str(), name.size() + 1);
	strncpy(*counter_description, description.c_str(), description.size() + 1);
	*verbosity_level = (*counter[counter_id]).getVerbosityLevel();
	
	return GASPI_SUCCESS;
}

gaspi_return_t
gaspi_statistic_counter_get ( gaspi_statistic_counter_t counter_id
			, gaspi_number_t argument
			, gaspi_number_t* value
			)
{
	assert(counter_id < COUNTER_NUMBER_MAX);

	*value = (*counter[counter_id]).getValue(argument);
	
	return GASPI_SUCCESS;
}
			
gaspi_return_t
gaspi_statistic_counter_reset (gaspi_statistic_counter_t counter_id)
{
	assert(counter_id < COUNTER_NUMBER_MAX);
	
	(*counter[counter_id]).reset();
	
	return GASPI_SUCCESS;
}

