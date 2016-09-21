#ifndef GASPI_TYPES_H
#define GASPI_TYPES_H

typedef enum
  {
    GASPI_NOT_USED = -1
  } gaspi_network_t;

typedef enum 
  {
    GASPI_ALLOC_DEFAULT = 0
  } gaspi_alloc_t;

// gaspi_return_t
typedef enum
  {
    GASPI_ERROR = -1,
    GASPI_SUCCESS = 0,
    GASPI_TIMEOUT = 1
  } gaspi_return_t;

// gaspi_state_t
typedef enum
  {
    GASPI_STATE_HEALTHY = 0,
    GASPI_STATE_CORRUPT = 1
  } gaspi_state_t;

// gaspi_statistic_argument_t
typedef enum
  {
    GASPI_STATISTIC_ARGUMENT_NONE,
    GASPI_STATISTIC_ARGUMENT_RANK
  } gaspi_statistic_argument_t;

// gaspi_atomic_value_t
typedef unsigned long gaspi_atomic_value_t;

// gaspi_const_pointer_t
typedef void* gaspi_const_pointer_t;

// gaspi_group_t
typedef unsigned short gaspi_group_t;

// gaspi_memory_description_t
typedef void* gaspi_memory_description_t;

// gaspi_network_t *network_t
typedef unsigned int gaspi_network_t;

// gaspi_notification_id_t
typedef unsigned short gaspi_notification_id_t;

// gaspi_notification_t
typedef unsigned int gaspi_notification_t;

// gaspi_number_t
typedef unsigned int gaspi_number_t;

// gaspi_offset_t
typedef unsigned long gaspi_offset_t;

// gaspi_pointer_t
typedef void* gaspi_pointer_t;

// gaspi_queue_id_t
typedef unsigned short gaspi_queue_id_t;

// gaspi_rank_t
typedef unsigned int gaspi_rank_t;

// gaspi_reduce_operation_t
typedef void* gaspi_reduce_operation_t;

// gaspi_reduce_state_t
typedef void* gaspi_reduce_state_t;

// gaspi_segment_id_t
typedef unsigned short gaspi_segment_id_t;

// gaspi_size_t
typedef unsigned long gaspi_size_t;

// gaspi_state_vector_t
typedef gaspi_state_t *gaspi_state_vector_t;

// gaspi_statistic_counter_t
typedef unsigned int gaspi_statistic_counter_t;

// gaspi_string_t
typedef char * gaspi_string_t;

// gaspi_timeout_t
typedef unsigned long gaspi_timeout_t;

// gaspi_time_t
typedef float gaspi_time_t;

// constants
static const gaspi_rank_t GASPI_GROUP_ALL = 0;
static const gaspi_timeout_t GASPI_BLOCK = -1;
static const gaspi_timeout_t GASPI_TEST = 0;

#endif

