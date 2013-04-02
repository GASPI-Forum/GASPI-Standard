#include "gaspi_gpi_profiler.h"
#include <iostream>
#include <assert.h>
#include <string.h>

map<gaspi_statistic_counter_t,Counter*> counter_map;

gaspi_number_t verbosity_level = 0;

gaspi_return_t
gaspi_proc_init ( gaspi_configuration_t configuration
                , gaspi_timeout_t timeout
                )
{
	counter_map[COUNTER_TOTAL_WRITE_VALUE] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_WRITE_VALUE", "Counts all written Bytes", 1, OPERATION_ADD);
	counter_map[COUNTER_TOTAL_WRITE_INVOC] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_WRITE_INVOC", "Counts all invocations for the write command", 2, OPERATION_ADD);
	counter_map[COUNTER_TOTAL_WRITE_MIN] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_WRITE_MIN", "Stores the minimum amount of written bytes", 2, OPERATION_MIN);
	counter_map[COUNTER_TOTAL_WRITE_MAX] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_WRITE_MAX", "Stores the maximum amount of written bytes", 2, OPERATION_MAX);
	counter_map[COUNTER_TOTAL_READ_VALUE] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_READ_VALUE", "Counts all read Bytes", 1, OPERATION_ADD);
	counter_map[COUNTER_TOTAL_READ_INVOC] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_READ_INVOC", "Counts all invocations for the read command", 2, OPERATION_ADD);
	counter_map[COUNTER_TOTAL_READ_MIN] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_READ_MIN", "Stores the minimum amount of read bytes", 2, OPERATION_MIN);
	counter_map[COUNTER_TOTAL_READ_MAX] = new CounterBasic(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_TOTAL_READ_MAX", "Stores the maximum amount of read bytes", 2, OPERATION_MAX);
	counter_map[COUNTER_RANK_WRITE_VALUE] = new CounterBasicRank(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_RANK_WRITE_VALUE", "Counts all written Bytes", 3);
	counter_map[COUNTER_RANK_READ_VALUE] = new CounterBasicRank(GASPI_STATISTIC_ARGUMENT_NONE, "COUNTER_RANK_READ_VALUE", "Counts all written Bytes", 3);
	
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
	gaspi_return_t status = pgaspi_write(segment_id_local, offset_local, 
		rank, segment_id_remote, offset_remote, size, queue, timeout);
	if(status == GASPI_SUCCESS && verbosity_level > 0)
	{
		(*counter_map[COUNTER_TOTAL_WRITE_VALUE]).add(0, size);
		if(verbosity_level > 1)
		{
			(*counter_map[COUNTER_TOTAL_WRITE_INVOC]).add(0, size);
			(*counter_map[COUNTER_TOTAL_WRITE_MIN]).add(0, size);
			(*counter_map[COUNTER_TOTAL_WRITE_MAX]).add(0, size);
			if(verbosity_level > 2)
			{
				(*counter_map[COUNTER_RANK_WRITE_VALUE]).add(rank, size);
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
	gaspi_return_t status = pgaspi_read(segment_id_local, offset_local, 
		rank, segment_id_remote, offset_remote, size, queue, timeout);
	if(status == GASPI_SUCCESS && verbosity_level > 0)
	{
		(*counter_map[COUNTER_TOTAL_READ_VALUE]).add(0, size);
		if(verbosity_level > 1)
		{
			(*counter_map[COUNTER_TOTAL_READ_INVOC]).add(0, size);
			(*counter_map[COUNTER_TOTAL_READ_MIN]).add(0, size);
			(*counter_map[COUNTER_TOTAL_READ_MAX]).add(0, size);
			if(verbosity_level > 2)
			{
				(*counter_map[COUNTER_RANK_READ_VALUE]).add(rank, size);
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
	*counter_max = counter_map.size();
	
	return GASPI_SUCCESS;
}

gaspi_return_t
gaspi_statistic_counter_info(gaspi_statistic_counter_t counter
			, gaspi_statistic_argument_t* counter_argument
			, gaspi_string_t* counter_name
			, gaspi_string_t* counter_description
			, gaspi_number_t* verbosity_level
			)
{
	assert(counter < counter_map.size());
	
	Counter* counter_temp = counter_map[counter];
	*counter_argument = (*counter_temp).getStatisticArgument();
	string name = (*counter_temp).getName();
	string description = (*counter_temp).getDescription();
	strncpy(*counter_name, name.c_str(), name.size() + 1);
	strncpy(*counter_description, description.c_str(), description.size() + 1);
	*verbosity_level = (*counter_temp).getVerbosityLevel();
	
	return GASPI_SUCCESS;
}

gaspi_return_t
gaspi_statistic_counter_get ( gaspi_statistic_counter_t counter
			, gaspi_number_t argument
			, gaspi_number_t* value
			)
{
	assert(counter < counter_map.size());

	*value = (*counter_map[counter]).getValue(argument);
	
	return GASPI_SUCCESS;
}
			
gaspi_return_t
gaspi_statistic_counter_reset (gaspi_statistic_counter_t counter)
{
	assert(counter < counter_map.size());
	
	(*counter_map[counter]).reset();
	
	return GASPI_SUCCESS;
}

