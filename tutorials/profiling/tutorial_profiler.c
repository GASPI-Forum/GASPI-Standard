#include "GASPI.h" 
#include <stdio.h>  /* needed for printf */

/*
 * In this tutorial we implement two statistical counters:
 * one counting the number of bytes written to remote nodes 
 * and one counting the number of bytes read from remote nodes.
 * The counter ids are defined in terms of an enumeration:
 */
enum {
	COUNTER_BYTES_WRITTEN,
	COUNTER_BYTES_READ,
	COUNTER_NUMBER_MAX       
}; 

/*
 * The reimplementation of gaspi_statistic_counter_max informs the
 * application programmer, that there are newly available counters.
 * The call to pgaspi_statistic_counter_max ensures, that our counters
 * don't interfere with counters provided by the underlying GASPI library.
 */
gaspi_return_t
gaspi_statistic_counter_max ( gaspi_number_t *counter_max )
{
  gaspi_return_t result = pgaspi_statistic_counter_max(counter_max); 
  if (result == GASPI_SUCCESS)
  {
    *counter_max += COUNTER_NUMBER_MAX;
  }
  return result;
}

/*
 * An array of counter_info_t provides counter information.
 */
typedef struct {
  /*const*/ gaspi_statistic_argument_t argument_;
  /*const*/ gaspi_string_t  name_;
  /*const*/ gaspi_string_t  desc_;
  /*const*/ gaspi_number_t  verbosity_level_;
  gaspi_number_t  value_;              /* the current value of the counter */
} counter_info_t;

static counter_info_t g_counter_info[COUNTER_NUMBER_MAX] = {
  { GASPI_STATISTIC_ARGUMENT_NONE, 
    "BytesWritten", 
    "Number of bytes written by this rank", 
    1, 
    0 
  },
  { GASPI_STATISTIC_ARGUMENT_NONE, 
    "BytesRead", 
    "Number of bytes read by this rank", 
    1 , 
    0 
  }
};


/*
 * The reimplementation of gaspi_statistic_counter_info informs the
 * application programmer about the counter properties. Again, it respects
 * counters provided by the underlying GASPI library.
 */
gaspi_return_t
gaspi_statistic_counter_info(gaspi_statistic_counter_t counter_id
			, gaspi_statistic_argument_t* counter_argument
			, gaspi_string_t* counter_name
			, gaspi_string_t* counter_description
			, gaspi_number_t* verbosity_level)
{
  gaspi_number_t counter_max;
  gaspi_return_t result = pgaspi_statistic_counter_max(counter_max); 
  if (result != GASPI_SUCCESS)
  {
    return result;
  }

  if (counter_id < counter_max)
  {
    /* The counter id belongs to the native GASPI library */
    return pgaspi_statistic_counter_info(counter_id, 
                                         counter_argument, 
                                         counter_name, 
                                         counter_description, 
                                         verbosity_level);
  }
  counter_id -= counter_max;
  if (counter_id < COUNTER_NUMBER_MAX)
  {
    *counter_argument = g_counter_info[counter_id].argument_;
    *counter_name = g_counter_info[counter_id].name_;
    *counter_description = g_counter_info[counter_id].desc_;
    *verbosity_level = g_counter_info[counter_id].verbosity_level_;			 		  
    return GASPI_SUCCESS;
  }
  else
  {
    /* The counter id neither belongs to the native library nor to this 
     * wrapper. In this case the standard requires to return GASPI_ERROR.
     * It is not allowed to exit (e.g. due to an assertion) in this case.
     */
    return GASPI_ERROR;
  }
}

/*
 * Retrieving and resetting the current counter values is also implemented 
 * by accessing the according counter_info_t structure. The handling of
 * counter ids which do not belong to this wrapper is equal to the handling
 * in gaspi_statistic_counter_info.
 */
gaspi_return_t
gaspi_statistic_counter_get ( gaspi_statistic_counter_t counter_id
			, gaspi_number_t argument
			, gaspi_number_t* value)
{
  gaspi_number_t counter_max;
  gaspi_return_t result = pgaspi_statistic_counter_max(counter_max); 
  if (result != GASPI_SUCCESS)
  {
    return result;
  }

  if (counter_id < counter_max)
  {
    return pgaspi_statistic_counter_get(counter_id, argument, value);
  }
  counter_id -= counter_max;
  if (counter_id < COUNTER_NUMBER_MAX)
  {
    *value = g_counter_info[counter_id].value_;
    return GASPI_SUCCESS;
  }
  else
  {
    return GASPI_ERROR;
  }
}

			
gaspi_return_t
gaspi_statistic_counter_reset (gaspi_statistic_counter_t counter_id)
{
  gaspi_number_t counter_max;
  gaspi_return_t result = pgaspi_statistic_counter_max(counter_max); 
  if (result != GASPI_SUCCESS)
  {
    return result;
  }

  if (counter_id < counter_max)
  {
    return pgaspi_statistic_counter_reset(counter_id);
  }
  counter_id -= counter_max;
  if (counter_id < COUNTER_NUMBER_MAX)
  {
    g_counter_info[counter_id].value_ = 0;
    return GASPI_SUCCESS;
  }
  else
  {
    return GASPI_ERROR;
  }
}

/* 
 * A GASPI library is not required to keep track of the current verbosity
 * level. Thus the wrapper itself has to keep track of it. GASPI does not 
 * require an initial verbosity level (e.g. of 0). In this example the 
 * statistical counters are active by default.
 */ 
static gaspi_number_t g_verbosity_level = 1;

gaspi_return_t
gaspi_statistic_verbosity_level(gaspi_number_t verbosity_level)
{
  g_verbosity_level = verbosity_level;
  return pgaspi_statistic_verbosity_level(verbosity_level);
}


/* 
 * The COUNTER_BYTES_WRITTEN counter is implemented by wrapping the 
 * gaspi_write procedure and count the number of bytes, when the underlying
 * library routine returns GASPI_SUCCESS. In this example only gaspi_write 
 * is wrapped. A complete wrapper has also to wrap gaspi_write_notify, 
 * gaspi_write_list and gaspi_write_list_notify.
 */
gaspi_return_t
gaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_id_t queue
            , gaspi_timeout_t timeout)
{
  /* first: call the native rountine, which is doing all the work */
  gaspi_return_t result = pgaspi_write(segment_id_local, offset_local, 
    rank, segment_id_remote, offset_remote, size, queue, timeout);

  /* then count... */		
  if (g_verbosity_level >= 1) 
  {
    /* ...but only, if an appropriate verbositiy level is set... */
    if (result == GASPI_SUCCESS)
    {
      /* ...and count only bytes written successfully */
      g_counter_info[COUNTER_BYTES_WRITTEN].value_ += size;
    }
  }
  return result;
}


/* 
 * The COUNTER_BYTES_READ counter is implemented by wrapping the 
 * gaspi_read procedure and count the number of bytes, when the underlying
 * library routine returns GASPI_SUCCESS. In this example only gaspi_read 
 * is wrapped. A complete wrapper has also to wrap gaspi_read_list.
 */
gaspi_return_t
gaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout)
{
  /* first: call the native rountine, which is doing all the work */
  gaspi_return_t result = pgaspi_read(segment_id_local, offset_local, 
    rank, segment_id_remote, offset_remote, size, queue, timeout);
	
  /* then count... */		
  if (g_verbosity_level >= 1) 
  {
    /* ...but only, if an appropriate verbositiy level is set... */
    if (result == GASPI_SUCCESS)
    {
      /* ...and count only bytes read successfully */
      g_counter_info[COUNTER_BYTES_READ].value_ += size;
    }
  }
  return result;
}

/* 
 * The output of the counter values can be left to the application programmer. 
 * However it is also possible to output the counter values from inside the 
 * wrapper library itself. A good way is to wrap a function, which hallmarks a 
 * particular execution state. In this example we have choosen gaspi_proc_term, 
 * but gaspi_barrier could be a good candidate too.
 */ 
gaspi_return_t
gaspi_proc_term( gaspi_timeout_t timeout )
{
  /* 
   * if we need the rank for our output (e.g. if we want to incorporate it in a
   * file name), we have to call gaspi_proc_rank before the shutdown procedure.
   */
  gaspi_rank_t rank;
  int i;
  gaspi_proc_rank (&rank);   /* expects always success */

  /* call the native routine, which is doing the shutdown */
  gaspi_return_t result = pgaspi_proc_term(timeout);
  
  if (result != GASPI_TIMEOUT) 
  {  
    /*
     * print the counter values to stdio. There are other output options too.
     * E.g. it is possible to print to stderr using fprintf(stderr, ...);
     */ 
    for (i = 0; i < COUNTER_NUMBER_MAX; ++i)
    {
      printf("%s (rank %d): %d\n", g_counter_info[i].desc_, 
                                   rank,
                                   g_counter_info[i].value_);
    }
  }
  return result; 
}
