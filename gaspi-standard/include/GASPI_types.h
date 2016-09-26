#ifndef GASPI_TYPES_H
#define GASPI_TYPES_H

/*
 * ENUMERATION:
 *   gaspi_network_t
 *
 * DESCRIPTON:
 *   the GASPI network infrastructure type.
 */
typedef enum
  {
    GASPI_NOT_USED = -1
  } gaspi_network_t;

/*
 * ENUMERATION:
 *   gaspi_alloc_t
 *
 * DESCRIPTION:
 *   the policy by which memory is allocated.
 */
typedef enum 
  {
    GASPI_ALLOC_DEFAULT = 0
  } gaspi_alloc_t;

/*
 * ENUMERATION:
 *   gaspi_return_t
 *
 * DESCRIPTION:
 *   the GASPI return value.
 */
typedef enum
  {
    GASPI_ERROR = -1,
    GASPI_SUCCESS = 0,
    GASPI_TIMEOUT = 1
  } gaspi_return_t;

/*
 * ENUMERATION:
 *   gaspi_state_t
 *  
 * DESCRIPTION:
 *   the health state of a remote GASPI process.
 */
typedef enum
  {
    GASPI_STATE_HEALTHY = 0,
    GASPI_STATE_CORRUPT = 1
  } gaspi_state_t;

/*
 * ENUMERATION:
 *   gaspi_statistic_argument_t
 * 
 * DESCRIPTION:
 *   the meaning of the argument value for the statistic counters.  
 */
typedef enum
  {
    GASPI_STATISTIC_ARGUMENT_NONE,
    GASPI_STATISTIC_ARGUMENT_RANK
  } gaspi_statistic_argument_t;

/*
 * TYPE:
 *   gaspi_atomic_value_t
 *
 * DESCRIPTION:
 *   the value used in atomic global GASPI operations.
 */ 
typedef unsigned long gaspi_atomic_value_t;

/*
 * TYPE:
 *   gaspi_const_pointer_t
 *
 * DESCRIPTION:
 *   a const type that can point to some memory.
 */
typedef void* gaspi_const_pointer_t;

/*
 * TYPE:
 *   gaspi_group_t  
 *
 * DESCRIPTION:
 *   the GASPI group type.
 */
typedef unsigned short gaspi_group_t;

/*
 * TYPE:
 *   gaspi_memory_description_t
 *
 * DESCRIPTION:
 *   the memory description for externally allocated memory 
 */
typedef void* gaspi_memory_description_t;

/*
 * TYPE:
 *   gaspi_notification_id_t
 *
 * DESCRIPTION:
 *   the GASPI notification ID type.
 */
typedef unsigned short gaspi_notification_id_t;

/*
 * TYPE:
 *   gaspi_notification_t
 *
 * DESCRIPTION:
 *   the GASPI notification value type.
 */
typedef unsigned int gaspi_notification_t;

/*
 * TYPE:
 *   gaspi_number_t
 *
 * DESCRIPTION:
 *   a type that is used to count elements. That could be numbers.
 */
typedef unsigned int gaspi_number_t;

/* 
 * TYPE: 
 *   gaspi_offset_t
 *
 * DESCRIPTION:
 *   the GASPI offset type. Offset are relative to the pointer to a segment 
 */
typedef unsigned long gaspi_offset_t;

/*
 * TYPE:
 *   gaspi_pointer_t
 *
 * DESCRIPTION:
 *   a type that can point to some memory.
 */
typedef void* gaspi_pointer_t;

/*
 * TYPE:
 *   gaspi_queue_id_t
 *
 * DESCRIPTION:
 *   the GASPI queue ID.
 */
typedef unsigned short gaspi_queue_id_t;

/*
 * TYPE:
 *   gaspi_rank_t
 *
 * DESCRIPTION:
 *   the GASPI rank type.
 */
typedef unsigned int gaspi_rank_t;

/*
 * TYPE:
 *   gaspi_reduce_operation_t
 *
 * DESCRIPTION:
 *   the GASPI reduction operation type
 */
typedef void* gaspi_reduce_operation_t;

/*
 * TYPE:
 *   gaspi_reduce_state_t
 *
 * DESCRIPTION:
 *   the GASPI reduction operation state type
 */
typedef void* gaspi_reduce_state_t;

/*
 * TYPE:
 *   gaspi_segment_id_t
 *
 * DESCRIPTION:
 *   the GASPI memory segment ID type
 */
typedef unsigned short gaspi_segment_id_t;

/*
 * TYPE:
 *   gaspi_size_t
 *
 * DESCRIPTION:
 *   the GASPI size type. Sizes are measured in units of bytes
 */
typedef unsigned long gaspi_size_t;

/*
 * TYPE:
 *   gaspi_state_vector_t
 *
 * DESCRIPTION:
 *   an array which contains the healthy state of all nodes.
 */
typedef gaspi_state_t *gaspi_state_vector_t;

/*
 * TYPE:
 *   gaspi_statistic_counter_t
 *
 * DESCRIPTION:
 *   the GASPI statistic counter ID
 */
typedef unsigned int gaspi_statistic_counter_t;

/*
 * TYPE:
 *   gaspi_string_t
 *
 * DESCRIPTION
 *   a string. used e.g. in error messages.
 */
typedef char * gaspi_string_t;

/*
 * TYPE:
 *   gaspi_timeout_t
 *
 * DESCRIPTION:
 *   the GASPI timeout value (predfined - GASPI_BLOCK, GAPSI_TEST) or milliseconds.
 */
typedef unsigned long gaspi_timeout_t;

/*
 * TYPE:
 *   gaspi_time_t
 *
 * DESCRIPTION:
 *   the GASPI wallclock time.
 */
typedef float gaspi_time_t;


// constants
static const gaspi_rank_t GASPI_GROUP_ALL = 0;
static const gaspi_timeout_t GASPI_BLOCK = -1;
static const gaspi_timeout_t GASPI_TEST = 0;

#endif

