module GASPI_types
  
  use, intrinsic :: ISO_C_BINDING
  
!
! ENUMERATION:
!   gaspi_network_t
!
! DESCRIPTON:
!   the GASPI network infrastructure type.
!
  integer, parameter   :: gaspi_network_t = c_int
  enum, bind(C)
     enumerator :: GASPI_NOT_USED = -1
  end enum
 
!
! ENUMERATION:
!   gaspi_alloc_t
!
! DESCRIPTION:
!   the policy by which memory is allocated.
!
  integer, parameter   :: gaspi_alloc_t = c_int
  enum, bind(C)
     enumerator :: GASPI_MEM_UNINITIALIZED = 0
     enumerator :: GASPI_ALLOC_DEFAULT = 0
  end enum

!
! ENUMERATION:
!   gaspi_return_t
!
! DESCRIPTION:
!   the GASPI return value.
!
  integer, parameter   :: gaspi_return_t = c_int
  enum, bind(C)
     enumerator :: GASPI_ERROR = -1
     enumerator :: GASPI_SUCCESS = 0
     enumerator :: GASPI_TIMEOUT = 1
  end enum

!
! ENUMERATION:
!   gaspi_state_t
!  
! DESCRIPTION:
!   the health state of a remote GASPI process.
!
  integer, parameter   :: gaspi_state_t = c_int
  enum, bind(C)
     enumerator :: GASPI_STATE_HEALTHY = 0
     enumerator :: GASPI_STATE_CORRUPT = 1
  end enum

!
! ENUMERATION:
!   gaspi_statistic_argument_t
! 
! DESCRIPTION:
!   the meaning of the argument value for the statistic counters.  
!
  integer, parameter   :: gaspi_statistic_argument_t = c_int
  enum, bind(C)
     enumerator :: GASPI_STATISTIC_ARGUMENT_NONE = 0 
     enumerator :: GASPI_STATISTIC_ARGUMENT_RANK = 1
  end enum

!
! TYPE:
!   gaspi_atomic_value_t
!
! DESCRIPTION:
!   the value used in atomic global GASPI operations.
! 
  integer, parameter   :: gaspi_atomic_value_t = c_long

!
! TYPE:
!   gaspi_group_t  
!
! DESCRIPTION:
!   the GASPI group type.
!
  integer, parameter   :: gaspi_group_t = c_signed_char

!
! TYPE:
!   gaspi_memory_description_t
!
! DESCRIPTION:
!   the memory description for externally allocated memory 
!
  integer, parameter   :: gaspi_memory_description_t = c_int

!
! TYPE:
!   gaspi_notification_id_t
!
! DESCRIPTION:
!   the GASPI notification ID type.
!
  integer, parameter   :: gaspi_notification_id_t = c_int

!
! TYPE:
!   gaspi_notification_t
!
! DESCRIPTION:
!   the GASPI notification value type.
!
  integer, parameter   :: gaspi_notification_t = c_int
  
!
! TYPE:
!   gaspi_number_t
!
! DESCRIPTION:
!   a type that is used to count elements. That could be numbers.
!
  integer, parameter   :: gaspi_number_t = c_int
  
! 
! TYPE: 
!   gaspi_offset_t
!
! DESCRIPTION:
!   the GASPI offset type. Offset are relative to the pointer to a segment 
!
  integer, parameter   :: gaspi_offset_t = c_long

!
! TYPE:
!   gaspi_queue_id_t
!
! DESCRIPTION:
!   the GASPI queue ID.
!
  integer, parameter   :: gaspi_queue_id_t = c_signed_char

!
! TYPE:
!   gaspi_rank_t
!
! DESCRIPTION:
!   the GASPI rank type.
!
  integer, parameter   :: gaspi_rank_t = c_short
  
!
! TYPE:
!   gaspi_segment_id_t
!
! DESCRIPTION:
!   the GASPI memory segment ID type
!
  integer, parameter   :: gaspi_segment_id_t = c_signed_char
  
!
! TYPE:
!   gaspi_size_t
!
! DESCRIPTION:
!   the GASPI size type. Sizes are measured in units of bytes
!
  integer, parameter   :: gaspi_size_t = c_long
  
!
! TYPE:
!   gaspi_statistic_counter_t
!
! DESCRIPTION:
!   the GASPI statistic counter ID
!
  integer, parameter   :: gaspi_statistic_counter_t = c_int

!
! TYPE:
!   gaspi_string_t
!
! DESCRIPTION
!   a string. used e.g. in error messages.
!
  integer, parameter :: gaspi_char = c_char
  
!
! TYPE:
!   gaspi_timeout_t
!
! DESCRIPTION:
!   the GASPI timeout value (predefined - GASPI_BLOCK, GAPSI_TEST) or milliseconds.
!
  integer, parameter   :: gaspi_timeout_t = c_int
  
!
! TYPE:
!   gaspi_time_t
!
! DESCRIPTION:
!   the GASPI wallclock time.
!
  integer, parameter   :: gaspi_time_t = c_long


! constants
  integer(gaspi_group_t), parameter :: GASPI_GROUP_ALL = 0
  integer(gaspi_group_t), parameter :: GASPI_BLOCK = -1
  integer(gaspi_group_t), parameter :: GASPI_TEST = 0
  

end module GASPI_types


