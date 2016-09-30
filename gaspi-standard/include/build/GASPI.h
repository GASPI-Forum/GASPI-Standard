#ifndef GASPI_H
#define GASPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "GASPI_types.h"

/*
 * Elementary GASPI types and implementation specific enumerations.
 *
 * TYPES:
 *   The elementary GASPI types, (gaspi_rank_t, gaspi_timeout_t, etc.) 
 *   used in the GASPI specification shall be part of the actual 
 *   implementation. The elementary GASPI types shall be included as part of an external 
 *   implementation header file.
 * 
 * ENUMERATIONS:
 *   The error code enumeration, policy enumeration, enumerations for configuration files
 *   and possible states for the state vector are specific for an actual implementation. 
 *   The enumerations shall be included as part of an external implementation header file.
 */

  
/* 
 * ENUMERATION:
 *   gaspi_operation_t
 *
 * DESCRIPTION:
 *   Collective communication operations. 
 *   The GASPI specification defines the following enumeration:
 *
 *   GASPI_OP_MIN - determines the minimum of the elements of each column of the input vector.
 *   GASPI_OP_MAX - determines the maximum of the elements of each column of the input vector.
 *   GASPI_OP_SUM - sums up all elements of each column of the input vector.
 */

typedef enum { GASPI_OP_MIN
             , GASPI_OP_MAX
             , GASPI_OP_SUM
             } gaspi_operation_t;

/*
 * ENUMERATION:
 *   gaspi_datatype_t
 *
 * DESCRIPTION:
 *   Predefined types for the allreduce operation 
 *   The GASPI specification defines the following enumeration:
 *
 *   GASPI_TYPE_INT - integer
 *   GASPI_TYPE_UINT - unsigned integer
 *   GASPI_TYPE_LONG - long
 *   GASPI_TYPE_ULONG - unsigned long
 *   GASPI_TYPE_FLOAT - float
 *   GASPI_TYPE_DOUBLE - double
 */

typedef enum { GASPI_TYPE_INT
             , GASPI_TYPE_UINT
             , GASPI_TYPE_LONG
             , GASPI_TYPE_ULONG
             , GASPI_TYPE_FLOAT
             , GASPI_TYPE_DOUBLE
             } gaspi_datatype_t;

/*
 * TYPE:
 *   gaspi_config_t
 *
 * DESCRIPTION:
 *   Predefined type which contains the GASPI config values.
 *   These can be set, retrieved by gaspi_config_set and gaspi_config_get respectively.
 *
 *   - group_max - the desired maximum number of permissible groups per process. 
 *     There is a hardware/implementation dependent maximum.
 *
 *   - segment_max - the desired number of maximally permissible 
 *     segments per GASPI process. There is a hardware/implementation dependent maximum.
 * 
 *   - queue_num - the desired number of one-sided communication queues to be created. 
 *     There is a hardware/implementation dependent maximum.
 *
 *   - queue_size_max - the desired number of simultaneously allowed on-going requests on a 
 *     one-sided communication queue. There is a hardware/implementation dependent maximum.
 *
 *   - transfer_size_max - the desired maximum size of a single data
 *     transfer in the one-sided communication channel.
 *     There is a hardware/implementation dependent maximum.
 *
 *   - notification_num - the desired number of internal notification buffers for weak 
 *     synchronisation to be created. There is a hardware/implementation dependent maximum.
 *
 *   - passive_queue_size_max - the desired number of simultaneously allowed on-going requests on the
 *     passive communication queue. There is a hardware/implementation dependent maximum.
 *
 *   - passive_transfer_size_max - the desired maximum size of a
 *     single data transfer in the passive communication channel.
 *     There is a hardware/implementation dependent maximum.
 *
 *   - allreduce_elem_max - the maximum number of elements in gaspi_allreduce.
 *     There is a hardware/implementation dependent maximum.
 *
 *   - allreduce_buf_size - the size of the internal buffer of gaspi_allreduce_user.
 *     There is a hardware/implementation dependent maximum.
 *
 *   - network - the network type to be used.
 *
 *   - build_infrastructure - indicates whether the communication infrastructure should
 *     be built up at startup time. The default value is true.
 *
 *   - user_defined - some user defined information that is application / implementation dependent.
 */

typedef struct {
  gaspi_number_t     group_max;
  gaspi_number_t     segment_max;
  gaspi_number_t     queue_num;
  gaspi_number_t     queue_size_max;
  gaspi_size_t       transfer_size_max;
  gaspi_number_t     notification_num;
  gaspi_number_t     passive_queue_size_max;
  gaspi_size_t       passive_transfer_size_max;
  gaspi_size_t       allreduce_buf_size;
  gaspi_number_t     allreduce_elem_max;
  gaspi_network_t    network;
  gaspi_number_t     build_infrastructure;
  void *             user_defined;
} gaspi_config_t;

  
/*
 * FUNCTION:
 *   config_get
 *
 * DESCRIPTION:
 *   The gaspi_config_get procedure is a synchronous local 
 *   blocking procedure which retrieves the default configuration 
 *   structure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_CONFIG_GET ( config )
 *
 * PARAMETER:
 *   @param config the default configuration  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_config_get ( gaspi_config_t *config );

/*
 * FUNCTION:
 *   config_set
 *
 * DESCRIPTION:
 *   The gaspi_config_set procedure is a synchronous local 
 *   blocking procedure which sets the configuration structure for 
 *   process initialization. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_CONFIG_SET ( config )
 *
 * PARAMETER:
 *   @param config the configuration structure to be set  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_config_set ( gaspi_config_t const config );

/*
 * FUNCTION:
 *   proc_init
 *
 * DESCRIPTION:
 *   gaspi_proc_init implements the GASPI initialization 
 *   of the application. It is a non-local synchronous time-based 
 *   blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PROC_INIT ( timeout )
 *
 * PARAMETER:
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_proc_init ( gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   proc_num
 *
 * DESCRIPTION:
 *   The total number of GASPI processes started, can be retrieved 
 *   by gaspi_proc_num. This is a local synchronous blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PROC_NUM ( proc_num )
 *
 * PARAMETER:
 *   @param proc_num the total number of GASPI processes  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_proc_num ( gaspi_rank_t *proc_num );

/*
 * FUNCTION:
 *   proc_rank
 *
 * DESCRIPTION:
 *   A rank identifies a GASPI process. The rank of a process lies 
 *   in the interval [0,P) where P can be retrieved through gaspi_proc_num. 
 *   Each process has a unique rank associated with it. The rank of 
 *   the invoking GASPI process can be retrieved by gaspi_proc_rank. 
 *   It is a local synchronous blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PROC_RANK ( rank )
 *
 * PARAMETER:
 *   @param rank the rank of the calling GASPI process.  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_proc_rank ( gaspi_rank_t *rank );

/*
 * FUNCTION:
 *   proc_term
 *
 * DESCRIPTION:
 *   The shutdown procedure gaspi_proc_term is a synchronous 
 *   non-local time-based blocking operation that releases resources 
 *   and performs the required clean\-up. There is no definition in 
 *   the specification of a verification of a healthy global state 
 *   (i.\,e.\ all processes terminated correctly). After a shutdown 
 *   call on a given GASPI process, it is undefined behavior if another 
 *   GASPI process tries to use any non-local GASPI functionality 
 *   involving that process{}. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PROC_TERM ( timeout )
 *
 * PARAMETER:
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_proc_term ( gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   proc_kill
 *
 * DESCRIPTION:
 *   gaspi_proc_kill sends an interrupt signal to a given 
 *   GASPI process. It is a synchronous non-local time-based blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PROC_KILL ( rank
 *                   , timeout )
 *
 * PARAMETER:
 *   @param rank the rank of the process to be killed  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_proc_kill ( gaspi_rank_t const rank
                , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   connect
 *
 * DESCRIPTION:
 *   In order to be able to communicate between two GASPI processes, 
 *   the communication infrastructure has to be established. This 
 *   is achieved with the synchronous non-local time-based blocking 
 *   procedure gaspi_connect. It is bound to the working 
 *   phase of the GASPI life cycle. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_CONNECT ( rank
 *                 , timeout )
 *
 * PARAMETER:
 *   @param rank the remote rank with which the communication infrastructure 
 *          is established  (in)
 *   @param timeout The timeout for the operation  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_connect ( gaspi_rank_t const rank
              , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   disconnect
 *
 * DESCRIPTION:
 *   The gaspi_disconnect procedure is a synchronous local 
 *   blocking procedure which disconnects a given process, identified 
 *   by its rank, and frees all associated resources. It is bound 
 *   to the working phase of the GASPI life cycle. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_DISCONNECT ( rank
 *                    , timeout )
 *
 * PARAMETER:
 *   @param rank the remote rank from which the communication infrastructure 
 *          is disconnected  (in)
 *   @param timeout The timeout for the operation  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_disconnect ( gaspi_rank_t const rank
                 , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   state_vec_get
 *
 * DESCRIPTION:
 *   The state vector is obtained by the local synchronous blocking 
 *   function gaspi_state_vec_get. The state vector represents 
 *   the states of all GASPI processes. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATE_VEC_GET ( state_vector )
 *
 * PARAMETER:
 *   @param returns the vector with individual return codes  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_state_vec_get ( gaspi_state_vector_t *state_vector );

/*
 * FUNCTION:
 *   group_create
 *
 * DESCRIPTION:
 *   The gaspi_group_create procedure is a synchronous 
 *   local blocking procedure which creates an empty group. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_CREATE ( group )
 *
 * PARAMETER:
 *   @param group the created empty group  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_create ( gaspi_group_t *group );

/*
 * FUNCTION:
 *   group_add
 *
 * DESCRIPTION:
 *   The gaspi_group_add procedure is a synchronous local 
 *   blocking procedure which adds a given rank to an existing group. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_ADD ( group
 *                   , rank )
 *
 * PARAMETER:
 *   @param group the group to which the rank is added  (inout)
 *   @param rank the rank to add to the group  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_add ( gaspi_group_t group
                , gaspi_rank_t const rank );

/*
 * FUNCTION:
 *   group_commit
 *
 * DESCRIPTION:
 *   The gaspi_group_commit procedure is a synchronous 
 *   collective time-based blocking procedure which establishes a 
 *   group. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_COMMIT ( group
 *                      , timeout )
 *
 * PARAMETER:
 *   @param group the group to commit  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_group_commit ( gaspi_group_t const group
                   , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   group_delete
 *
 * DESCRIPTION:
 *   The gaspi_group_delete procedure is a synchronous 
 *   local blocking procedure which deletes a given group. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_DELETE ( group )
 *
 * PARAMETER:
 *   @param group the group to be deleted  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_delete ( gaspi_group_t const group );

/*
 * FUNCTION:
 *   group_num
 *
 * DESCRIPTION:
 *   The gaspi_group_num procedure is a synchronous local 
 *   blocking procedure which returns the current number of allocated 
 *   groups. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_NUM ( group_num )
 *
 * PARAMETER:
 *   @param group_num the current number of groups  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_num ( gaspi_number_t *group_num );

/*
 * FUNCTION:
 *   group_size
 *
 * DESCRIPTION:
 *   The gaspi_group_size procedure is a synchronous local 
 *   blocking procedure which returns the number of ranks of a given 
 *   group. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_SIZE ( group
 *                    , group_size )
 *
 * PARAMETER:
 *   @param group the group to be examined  (in)
 *   @param group_size the number of ranks in a given group  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_size ( gaspi_group_t const group
                 , gaspi_number_t *group_size );

/*
 * FUNCTION:
 *   group_ranks
 *
 * DESCRIPTION:
 *   The gaspi_group_ranks procedure is a synchronous local 
 *   blocking procedure which returns a list of ranks of GASPI processes 
 *   forming the group. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_RANKS ( group
 *                     , group_ranks[group_size] )
 *
 * PARAMETER:
 *   @param group the group to be examined  (in)
 *   @param group_ranks the list of ranks forming the group  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_ranks ( gaspi_group_t const group
                  , gaspi_rank_t *group_ranks );

/*
 * FUNCTION:
 *   segment_alloc
 *
 * DESCRIPTION:
 *   The synchronous local blocking procedure gaspi_segment_alloc 
 *   allocates a memory segment and optionally maps it in accordance 
 *   with a given allocation policy. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_ALLOC ( segment_id
 *                       , size
 *                       , alloc_policy )
 *
 * PARAMETER:
 *   @param segment_id The segment ID to be created. The segment IDs need 
 *          to be unique on each GASPI process  (in)
 *   @param size The size of the segment in bytes  (in)
 *   @param alloc_policy allocation policy  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_alloc ( gaspi_segment_id_t const segment_id
                    , gaspi_size_t const size
                    , gaspi_alloc_t const alloc_policy );

/*
 * FUNCTION:
 *   segment_register
 *
 * DESCRIPTION:
 *   In order to be used in a one-sided communication request on 
 *   an existing connection, a segment allocated by gaspi_segment_alloc 
 *   needs to be made visible and accessible for the other GASPI processes. 
 *   This is accomplished by the procedure gaspi_segment_register. 
 *   It is a synchronous non-local time-based blocking procedure. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_REGISTER ( segment_id
 *                          , rank
 *                          , timeout )
 *
 * PARAMETER:
 *   @param segment_id The segment ID to be registered. The segment ID's need 
 *          to be unique for each GASPI process  (in)
 *   @param rank The rank of the GASPI process which should register 
 *          the new segment  (in)
 *   @param timeout The timeout for the operation  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_segment_register ( gaspi_segment_id_t const segment_id
                       , gaspi_rank_t const rank
                       , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   segment_create
 *
 * DESCRIPTION:
 *   gaspi_segment_create is a synchronous collective time-based 
 *   blocking procedure. It is semantically equivalent to a collective 
 *   aggregation of gaspi_segment_alloc, gaspi_segment_register 
 *   and gaspi_barrier involving all of the members of a 
 *   given group. If the communication infrastructure was not established 
 *   for all group members beforehand, gaspi_segment_create 
 *   will accomplish this as well. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_CREATE ( segment_id
 *                        , size
 *                        , group
 *                        , timeout
 *                        , alloc_policy )
 *
 * PARAMETER:
 *   @param segment_id The ID for the segment to be created. The segment ID's 
 *          need to be unique for each GASPI process  (in)
 *   @param size The size of the segment in bytes  (in)
 *   @param group The group which should create the segment  (in)
 *   @param timeout The timeout for the operation  (in)
 *   @param alloc_policy allocation policy  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_segment_create ( gaspi_segment_id_t const segment_id
                     , gaspi_size_t const size
                     , gaspi_group_t const group
                     , gaspi_timeout_t const timeout
                     , gaspi_alloc_t const alloc_policy );

/*
 * FUNCTION:
 *   segment_bind
 *
 * DESCRIPTION:
 *   The synchronous local blocking procedure gaspi_segment_bind 
 *   binds a segment id to user provided memory. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_BIND ( segment_id
 *                      , pointer
 *                      , size
 *                      , memory_description
 *                      )
 *
 * PARAMETER:
 *   @param segment_id Unique segment ID to bind.  (in)
 *   @param pointer The begin of the memory provided by the user.  (in)
 *   @param size The size of the memory provided by pointer in bytes. 
 *           (in)
 *   @param memory_description The description of the memory provided.  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t 
gaspi_segment_bind ( gaspi_segment_id_t const segment_id
                   , gaspi_pointer_t const pointer
                   , gaspi_size_t const size
                   , gaspi_memory_description_t const memory_description
                   );

/*
 * FUNCTION:
 *   segment_use
 *
 * DESCRIPTION:
 *   The synchronous collective time-based blocking procedure gaspi_segment_use 
 *   is semantically equivalent to a collective aggregation of gaspi_segment_bind, 
 *   gaspi_segment_register and gaspi_barrier 
 *   involving all members of a given group. If the communication 
 *   infrastructure was not established for all group members beforehand, 
 *   gaspi_segment_use will accomplish this as well. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_USE ( segment_id
 *                     , pointer
 *                     , size
 *                     , group
 *                     , timeout
 *                     , memory_description
 *                     )
 *
 * PARAMETER:
 *   @param segment_id Unique segment ID to bind.  (in)
 *   @param pointer The begin of the memory provided by the user.  (in)
 *   @param size The size of the memory provided by pointer in bytes. 
 *           (in)
 *   @param group The group which should create the segment.  (in)
 *   @param timeout The timeout for the operation.  (in)
 *   @param memory_description The description of the memory provided.  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t 
gaspi_segment_use ( gaspi_segment_id_t const segment_id
                  , gaspi_pointer_t const pointer
                  , gaspi_size_t const size
                  , gaspi_group_t const group
                  , gaspi_timeout_t const timeout
                  , gaspi_memory_description_t const memory_description
                  ) ;

/*
 * FUNCTION:
 *   segment_delete
 *
 * DESCRIPTION:
 *   The synchronous local blocking procedure gaspi_segment_delete 
 *   releases the resources of a previously allocated memory segment. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_DELETE ( segment_id )
 *
 * PARAMETER:
 *   @param segment_id The segment ID to be deleted.  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_delete ( gaspi_segment_id_t const segment_id );

/*
 * FUNCTION:
 *   segment_num
 *
 * DESCRIPTION:
 *   The gaspi_segment_num procedure is a synchronous local 
 *   blocking procedure which returns the current number of allocated 
 *   segments. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_NUM ( segment_num )
 *
 * PARAMETER:
 *   @param segment_num the current number of allocated segments  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_num ( gaspi_number_t *segment_num );

/*
 * FUNCTION:
 *   segment_list
 *
 * DESCRIPTION:
 *   The gaspi_segment_list procedure is a synchronous 
 *   local blocking procedure which returns a list of locally allocated 
 *   segment IDs. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_LIST ( num
 *                      , segment_id_list[num] )
 *
 * PARAMETER:
 *   @param num number of segment IDs to collect  (in)
 *   @param segment_list[num] list of locally allocated segment IDs  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_list ( gaspi_number_t const num
                   , gaspi_segment_id_t *segment_id_list );

/*
 * FUNCTION:
 *   segment_ptr
 *
 * DESCRIPTION:
 *   Segments are identified by a unique ID. This ID can be used 
 *   to obtain the virtual address of that local segment of memory. 
 *   The procedure gaspi_segment_ptr returns the pointer 
 *   to the segment represented by a given segment ID. It is a synchronous 
 *   local blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_PTR ( segment_id
 *                     , pointer )
 *
 * PARAMETER:
 *   @param segment_id The segment ID.  (in)
 *   @param pointer The pointer to the memory segment.  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_ptr ( gaspi_segment_id_t const segment_id
                  , gaspi_pointer_t *pointer );

/*
 * FUNCTION:
 *   write
 *
 * DESCRIPTION:
 *   The simplest form of a write operation is gaspi_write 
 *   which is a single communication call to write data to a remote 
 *   location. It is an asynchronous non-local time-based blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_WRITE ( segment_id_local
 *               , offset_local
 *               , rank
 *               , segment_id_remote
 *               , offset_remote
 *               , size
 *               , queue
 *               , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment ID to read from  (in)
 *   @param offset_local the local offset in bytes to read from  (in)
 *   @param rank the remote rank to write to  (in)
 *   @param segment_id_remote the remote segment to write to  (in)
 *   @param offset_remote the remote offset to write to  (in)
 *   @param size the size of the data to write  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_write ( gaspi_segment_id_t const segment_id_local
            , gaspi_offset_t const offset_local
            , gaspi_rank_t const rank
            , gaspi_segment_id_t const segment_id_remote
            , gaspi_offset_t const offset_remote
            , gaspi_size_t const size
            , gaspi_queue_id_t const queue
            , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   read
 *
 * DESCRIPTION:
 *   The simplest form of a read operation is gaspi_read 
 *   which is a single communication call to read data from a remote 
 *   location. It is an asynchronous non-local time-based blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_READ ( segment_id_local
 *              , offset_local
 *              , rank
 *              , segment_id_remote
 *              , offset_remote
 *              , size
 *              , queue
 *              , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment ID to write to  (in)
 *   @param offset_local the local offset in bytes to write to  (in)
 *   @param rank the remote rank to read from  (in)
 *   @param segment_id_remote the remote segment to read from  (in)
 *   @param offset_remote the remote offset to read from  (in)
 *   @param size the size of the data to read  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_read ( gaspi_segment_id_t const segment_id_local
           , gaspi_offset_t const offset_local
           , gaspi_rank_t const rank
           , gaspi_segment_id_t const segment_id_remote
           , gaspi_offset_t const offset_remote
           , gaspi_size_t const size
           , gaspi_queue_id_t const queue
           , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   wait
 *
 * DESCRIPTION:
 *   The gaspi_wait procedure is a time-based blocking 
 *   local procedure which waits until all one-sided communication 
 *   requests posted to a given queue are processed by the network 
 *   infrastructure. It is an asynchronous non-local time-based blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_WAIT ( queue
 *              , timeout )
 *
 * PARAMETER:
 *   @param queue the queue ID to wait for  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_wait ( gaspi_queue_id_t const queue
           , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   notify
 *
 * DESCRIPTION:
 *   gaspi_notify is an asynchronous non-local time-based 
 *   blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_NOTIFY ( segment_id
 *                , rank
 *                , notification_id
 *                , notification_value
 *                , queue
 *                , timeout )
 *
 * PARAMETER:
 *   @param segment_id the remote segment bound to the notification  (in)
 *   @param rank the remote rank to notify  (in)
 *   @param notification_id the remote notification ID  (in)
 *   @param notification_value the notification value (>0) to write  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_notify ( gaspi_segment_id_t const segment_id
             , gaspi_rank_t const rank
             , gaspi_notification_id_t const notification_id
             , gaspi_notification_t const notification_value
             , gaspi_queue_id_t const queue
             , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   notify_waitsome
 *
 * DESCRIPTION:
 *   For the procedures with notification, gaspi_notify 
 *   and the extendend functions gaspi_write_notify and 
 *   gaspi_read_notify, gaspi_notify_waitsome 
 *   is the correspondent wait procedure for the notified receiver 
 *   side (which is remote for the functions gaspi_notify 
 *   and gaspi_write_notify and local for the function gaspi_read_notify). 
 *   gaspi_notify_waitsome is a synchronous, non-local time-based 
 *   blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_NOTIFY_WAITSOME ( segment_id
 *                         , notification_begin
 *                         , notification_num
 *                         , first_id
 *                         , timeout )
 *
 * PARAMETER:
 *   @param segment_id the segment bound to the notification  (in)
 *   @param notification_begin the local notification ID for the first notification 
 *          to wait for  (in)
 *   @param notification_num the number of notification ID's to wait for  (in)
 *   @param first_id the id of the first notification that arrived  (out)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_notify_waitsome ( gaspi_segment_id_t const segment_id
                      , gaspi_notification_id_t notific_begin
                      , gaspi_number_t const notification_num
                      , gaspi_notification_id_t *first_id
                      , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   notify_reset
 *
 * DESCRIPTION:
 *   For the gaspi_notify_waitsome procedure, there is 
 *   a notification initialization procedure which resets the given 
 *   notification to zero. It is a synchronous local blocking procedure. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_NOTIFY_RESET ( segment_id
 *                      , notification_id
 *                      , old_notification_val )
 *
 * PARAMETER:
 *   @param segment_id the segment bound to the notification  (in)
 *   @param notification_id the local notification ID to reset  (in)
 *   @param old_notification_val notification value before reset  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_notify_reset ( gaspi_segment_id_t const segment_id
                   , gaspi_notification_id_t const notification_id
                   , gaspi_notification_t *old_notification_val);

/*
 * FUNCTION:
 *   write_notify
 *
 * DESCRIPTION:
 *   The gaspi_write_notify variant extends the simple 
 *   gaspi_write with a notification on the remote side. 
 *   This applies to communication patterns that require tighter synchronisation 
 *   on data movement. The remote receiver of the data is notified 
 *   when the write is finished and can verify this through the respective 
 *   wait procedure. It is an asynchronous non-local time-based blocking 
 *   procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_WRITE_NOTIFY ( segment_id_local
 *                      , offset_local
 *                      , rank
 *                      , segment_id_remote
 *                      , offset_remote
 *                      , size
 *                      , notification_id
 *                      , notification_value
 *                      , queue
 *                      , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment ID to read from  (in)
 *   @param offset_local the local offset in bytes to read from  (in)
 *   @param rank the remote rank to write to  (in)
 *   @param segment_id_remote the remote segment to write to  (in)
 *   @param offset_remote the remote offset to write to  (in)
 *   @param size the size of the data to write  (in)
 *   @param notification_id the remote notification ID  (in)
 *   @param notification_value the value of the notification to write  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_write_notify ( gaspi_segment_id_t const segment_id_local
                   , gaspi_offset_t const offset_local
                   , gaspi_rank_t const rank
                   , gaspi_segment_id_t const segment_id_remote
                   , gaspi_offset_t const offset_remote
                   , gaspi_size_t const size
                   , gaspi_notification_id_t const notification_id
                   , gaspi_notification_t const notification_value
                   , gaspi_queue_id_t const queue
                   , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   write_list
 *
 * DESCRIPTION:
 *   The gaspi_write_list variant allows strided communication 
 *   where a list of different data locations are processed at once. 
 *   Semantically, it is equivalent to a sequence of calls to gaspi_write 
 *   but it should (if possible) be more efficient. It is an asynchronous 
 *   non-local time-based blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_WRITE_LIST ( num
 *                    , segment_id_local[num]
 *                    , offset_local[num]
 *                    , rank
 *                    , segment_id_remote[num]
 *                    , offset_remote[num]
 *                    , size[num]
 *                    , queue
 *                    , timeout )
 *
 * PARAMETER:
 *   @param num the number of elements to write  (in)
 *   @param segment_id_local list of local segment ID's to read from  (in)
 *   @param offset_local list of local offsets in bytes to read from  (in)
 *   @param rank the remote rank to write to  (in)
 *   @param segment_id_remote list of remote segments to write to  (in)
 *   @param offset_remote list of remote offsets to write to  (in)
 *   @param size list of sizes of the data to write  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_write_list ( gaspi_number_t const num
                 , gaspi_segment_id_t const *segment_id_local
                 , gaspi_offset_t const *offset_local
                 , gaspi_rank_t const rank
                 , gaspi_segment_id_t const *segment_id_remote
                 , gaspi_offset_t const *offset_remote
                 , gaspi_size_t const *size
                 , gaspi_queue_id_t const queue
                 , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   write_list_notify
 *
 * DESCRIPTION:
 *   The gaspi_write_list_notify operation performs strided 
 *   communication as gaspi_write_list but also includes 
 *   a notification that the remote receiver can use to ensure that 
 *   the communication step is completed. It is an asynchronous non-local 
 *   time-based blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_WRITE_LIST_NOTIFY
 *                   ( num
 *                   , segment_id_local[num]
 *                   , offset_local[num]
 *                   , rank
 *                   , segment_id_remote[num]
 *                   , offset_remote[num]
 *                   , size[num]
 *                   , notification_id
 *                   , notification_value
 *                   , queue
 *                   , timeout )
 *
 * PARAMETER:
 *   @param num the number of elements to write  (in)
 *   @param segment_id_local list of local segment ID's to read from  (in)
 *   @param offset_local list of local offsets in bytes to read from  (in)
 *   @param rank the remote rank to be write to  (in)
 *   @param segment_id_remote list of remote segments to write to  (in)
 *   @param offset_remote list of remote offsets to write to  (in)
 *   @param size list of sizes of the data to write  (in)
 *   @param notification_id the remote notification ID  (in)
 *   @param notification_value the value of the notification to write  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_write_list_notify
                ( gaspi_number_t const num
                , gaspi_segment_id_t const *segment_id_local
                , gaspi_offset_t const *offset_local
                , gaspi_rank_t const rank
                , gaspi_segment_id_t const *segment_id_remote
                , gaspi_offset_t const *offset_remote
                , gaspi_size_t const *size
                , gaspi_notification_id_t const notification_id
                , gaspi_notification_t const notification_value
                , gaspi_queue_id_t const queue
                , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   read_notify
 *
 * DESCRIPTION:
 *   The gaspi_read_notify variant extends the simple gaspi_read 
 *   with a notification on the local side. This applies to communication 
 *   patterns that require tighter synchronisation on data movement. 
 *   The local receiver of the data is notified when the read is finished 
 *   and can verify this through the procedure gaspi_waitsome. 
 *   It is an asynchronous non-local time-based blocking procedure. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_READ_NOTIFY (  segment_id_local
 *                      , offset_local
 *                      , rank
 *                      , segment_id_remote
 *                      , offset_remote
 *                      , size
 *                      , notification_id
 *                      , queue
 *                      , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment to write to  (in)
 *   @param offset_local the local offset to write to  (in)
 *   @param rank the remote rank to read from  (in)
 *   @param segment_id_remote the remote segment ID to read from  (in)
 *   @param offset_remote the remote offset in bytes to read from  (in)
 *   @param size the size of the data to read  (in)
 *   @param notification_id the local notification ID  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */
                                                                                                                                               
gaspi_return_t
gaspi_read_notify (  gaspi_segment_id_t const segment_id_local
                   , gaspi_offset_t const offset_local
                   , gaspi_rank_t const rank
                   , gaspi_segment_id_t const segment_id_remote
                   , gaspi_offset_t const offset_remote
                   , gaspi_size_t const size
                   , gaspi_notification_id_t const notification_id
                   , gaspi_queue_id_t const queue
                   , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   read_list
 *
 * DESCRIPTION:
 *   The gaspi_read_list variant allows strided communication 
 *   where a list of different data locations are processed at once. 
 *   Semantically, it is equivalent to a sequence of calls to gaspi_read 
 *   but it should (if possible) be more efficient. It is an asynchronous 
 *   non-local time-based blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_READ_LIST ( num
 *                   , segment_id_local[num]
 *                   , offset_local[num]
 *                   , rank
 *                   , segment_id_remote[num]
 *                   , offset_remote[num]
 *                   , size[num]
 *                   , queue
 *                   , timeout )
 *
 * PARAMETER:
 *   @param num the number of elements to read  (in)
 *   @param segment_id_local list of local segment ID's to write to  (in)
 *   @param offset_local list of local offsets in bytes to write to  (in)
 *   @param rank the remote rank to read from  (in)
 *   @param segment_id_remote list of remote segments to read from  (in)
 *   @param offset_remote list of remote offsets to read from  (in)
 *   @param size list of sizes of the data to read  (in)
 *   @param queue the queue to use  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_read_list ( gaspi_number_t const num
                , gaspi_segment_id_t const *segment_id_local
                , gaspi_offset_t const *offset_local
                , gaspi_rank_t const rank
                , gaspi_segment_id_t const *segment_id_remote
                , gaspi_offset_t const *offset_remote
                , gaspi_size_t const *size
                , gaspi_queue_id_t const queue
                , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   queue_create
 *
 * DESCRIPTION:
 *   The gaspi_queue_create procedure is a synchronous 
 *   non-local time-based blocking procedure which creates a new queue 
 *   for communication. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_CREATE ( queue
 *                      , timeout
 *                      )
 *
 * PARAMETER:
 *   @param queue the created queue  (out)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_queue_create ( gaspi_queue_id_t *queue
                   , gaspi_timeout_t const timeout
                   );

/*
 * FUNCTION:
 *   queue_delete
 *
 * DESCRIPTION:
 *   The gaspi_queue_delete procedure is a synchronous 
 *   non-local time-based blocking procedure which deletes a given 
 *   queue. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_DELETE ( queue )
 *
 * PARAMETER:
 *   @param queue the queue to delete  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_queue_delete ( gaspi_queue_id_t const queue );

/*
 * FUNCTION:
 *   queue_size
 *
 * DESCRIPTION:
 *   The gaspi_queue_size procedure is a synchronous local 
 *   blocking procedure which determines the number of open communication 
 *   requests posted to a given queue. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_SIZE ( queue
 *                    , queue_size )
 *
 * PARAMETER:
 *   @param queue the queue to probe  (in)
 *   @param queue_size the number of open requests posted to the queue  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_queue_size ( gaspi_queue_id_t const queue
                 , gaspi_number_t *queue_size );

/*
 * FUNCTION:
 *   queue_purge
 *
 * DESCRIPTION:
 *   The gaspi_queue_purge procedure is a synchronous local 
 *   time-based blocking procedure which purges a given queue. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_PURGE ( queue
 *                     , timeout )
 *
 * PARAMETER:
 *   @param queue the queue to purge  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_queue_purge ( gaspi_queue_id_t const queue
                  , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   passive_send
 *
 * DESCRIPTION:
 *   gaspi_passive_send is the routine called by the sender 
 *   side to engage in passive communication. It is an synchronous 
 *   non-local time-based blocking procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PASSIVE_SEND ( segment_id_local
 *                      , offset_local
 *                      , rank
 *                      , size
 *                      , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment ID from which the data is sent  (in)
 *   @param offset_local the local offset from which the data is sent  (in)
 *   @param rank the remote rank to which the data is sent  (in)
 *   @param size the size of the data to be sent  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_passive_send ( gaspi_segment_id_t const segment_id_local
                   , gaspi_offset_t const offset_local
                   , gaspi_rank_t const rank
                   , gaspi_size_t const size
                   , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   passive_receive
 *
 * DESCRIPTION:
 *   The synchronous non-local time-based blocking gaspi_passive_receive 
 *   is one of the routines called by the receiver side to engage 
 *   in passive communication. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PASSIVE_RECEIVE ( segment_id_local
 *                         , offset_local
 *                         , rank
 *                         , size
 *                         , timeout )
 *
 * PARAMETER:
 *   @param segment_id_local the local segment ID where to write the data  (in)
 *   @param offset_local the local offset where to write the data  (in)
 *   @param rank the remote rank from which the data is transferred 
 *           (out)
 *   @param size the size of the data to be received  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_passive_receive ( gaspi_segment_id_t const segment_id_local
                      , gaspi_offset_t const offset_local
                      , gaspi_rank_t *rank
                      , gaspi_size_t const size
                      , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   passive_queue_purge
 *
 * DESCRIPTION:
 *   The gaspi_passive_queue_purge procedure is a synchronous 
 *   local time-based blocking procedure which purges the passive 
 *   queue. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PASSIVE_QUEUE_PURGE (timeout)
 *
 * PARAMETER:
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_passive_queue_purge (gaspi_timeout_t const timeout);

/*
 * FUNCTION:
 *   atomic_fetch_add
 *
 * DESCRIPTION:
 *   The gaspi_atomic_fetch_add procedure is a synchronous 
 *   non-local time-based blocking procedure which atomically adds 
 *   a given value to a globally acessible value. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ATOMIC_FETCH_ADD ( segment_id
 *                          , offset
 *                          , rank
 *                          , value_add
 *                          , value_old
 *                          , timeout )
 *
 * PARAMETER:
 *   @param segment_id the segment ID where the value is located  (in)
 *   @param offset the offset where the value is located  (in)
 *   @param rank the rank where the value is located  (in)
 *   @param value_add the value which is to be added  (in)
 *   @param value_old the old value before the operation  (out)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_atomic_fetch_add ( gaspi_segment_id_t const segment_id
                       , gaspi_offset_t const offset
                       , gaspi_rank_t const rank
                       , gaspi_atomic_value_t const value_add
                       , gaspi_atomic_value_t *value_old
                       , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   atomic_compare_swap
 *
 * DESCRIPTION:
 *   The gaspi_atomic_compare_swap procedure is a synchronous 
 *   non-local time-based blocking procedure which atomically compares 
 *   the value of a global value against some user given value and 
 *   in case these are equal the old value is replaced by a new value. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ATOMIC_COMPARE_SWAP ( segment_id
 *                             , offset
 *                             , rank
 *                             , comparator
 *                             , value_new
 *                             , value_old
 *                             , timeout )
 *
 * PARAMETER:
 *   @param segment_id the segment ID where the value is located  (in)
 *   @param offset the offset where the value is located  (in)
 *   @param rank the rank where the value is located  (in)
 *   @param comparator the value which is compared to the remote value  (in)
 *   @param value_new the new value to which the remote location is set if 
 *          the result of the comparison is true  (in)
 *   @param value_old the value before the operation  (out)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_atomic_compare_swap ( gaspi_segment_id_t const segment_id
                          , gaspi_offset_t const offset
                          , gaspi_rank_t const rank
                          , gaspi_atomic_value_t const comparator
                          , gaspi_atomic_value_t const value_new
                          , gaspi_atomic_value_t *value_old
                          , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   barrier
 *
 * DESCRIPTION:
 *   The gaspi_barrier procedure is a collective time-based 
 *   blocking procedure. An implementation is free to provide it as 
 *   a synchronous or an asynchronous procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_BARRIER ( group
 *                 , timeout )
 *
 * PARAMETER:
 *   @param group the group of ranks which should participate in the 
 *          barrier  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_barrier ( gaspi_group_t const group
              , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   allreduce
 *
 * DESCRIPTION:
 *   The gaspi_allreduce procedure is a collective time-based 
 *   blocking procedure. An implementation is free to provide it as 
 *   a synchronous or an asynchronous procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ALLREDUCE ( buffer_send
 *                   , buffer_receive
 *                   , num
 *                   , operation
 *                   , datatype
 *                   , group
 *                   , timeout )
 *
 * PARAMETER:
 *   @param buffer_send pointer to the buffer where the input is placed  (in)
 *   @param buffer_receive pointer to the buffer where the result is placed  (in)
 *   @param num the number of elements to be reduced on each process 
 *           (in)
 *   @param operation the GASPI reduction operation type  (in)
 *   @param datatype the GASPI element type  (in)
 *   @param group the group of ranks which participate in the reduction 
 *          operation  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_allreduce ( gaspi_const_pointer_t const buffer_send
                , gaspi_pointer_t const buffer_receive
                , gaspi_number_t const num
                , gaspi_operation_t const operation
                , gaspi_datatype_t const datatype
                , gaspi_group_t const group
                , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   allreduce_user
 *
 * DESCRIPTION:
 *   The procedure gaspi_allreduce_user allows the user 
 *   to specify its own reduction operation. Only operations are supported 
 *   which are commutative and associative. It is a collective time-based 
 *   blocking procedure. An implementation is free to provide it as 
 *   a synchronous or an asynchronous procedure. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ALLREDUCE_USER ( buffer_send
 *                        , buffer_receive
 *                        , num
 *                        , size_element
 *                        , reduce_operation
 *                        , reduce_state
 *                        , group
 *                        , timeout )
 *
 * PARAMETER:
 *   @param buffer_send pointer to the buffer where the input is placed  (in)
 *   @param buffer_receive pointer to the buffer where the result is placed  (in)
 *   @param num the number of elements to be reduced on each process 
 *           (in)
 *   @param size_element Size in bytes of one element to be reduced  (in)
 *   @param reduce_operation pointer to the user defined reduction operation procedure 
 *           (in)
 *   @param reduce_state reduction state vector  (inout)
 *   @param group the group of ranks which participate in the reduction 
 *          operation  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_allreduce_user ( gaspi_const_pointer_t const buffer_send
                     , gaspi_pointer_t const buffer_receive
                     , gaspi_number_t const num
                     , gaspi_size_t const size_element
                     , gaspi_reduce_operation_t const reduce_operation
                     , gaspi_reduce_state_t reduce_state
                     , gaspi_group_t const group
                     , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   reduce_operation
 *
 * DESCRIPTION:
 *   The prototype for the user defined reduction operations is the 
 *   following: 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_REDUCE_OPERATION ( operand_one
 *                          , operand_two
 *                          , result
 *                          , state
 *                          , timeout )
 *
 * PARAMETER:
 *   @param operand_one pointer to the first operand  (in)
 *   @param operand_two pointer to the second operand  (in)
 *   @param result pointer to the result  (in)
 *   @param state pointer to the state  (in)
 *   @param timeout the timeout  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 *           GASPI_TIMEOUT in case of timeout.
 */

gaspi_return_t
gaspi_reduce_operation ( gaspi_const_pointer_t const operand_one
                       , gaspi_const_pointer_t const operand_two
                       , gaspi_pointer_t const result
                       , gaspi_reduce_state_t const state
                       , gaspi_timeout_t const timeout );

/*
 * FUNCTION:
 *   group_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_GROUP_MAX (group_max)
 *
 * PARAMETER:
 *   @param group_max the total number of groups  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_group_max (gaspi_number_t *group_max);

/*
 * FUNCTION:
 *   segment_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_SEGMENT_MAX (segment_max)
 *
 * PARAMETER:
 *   @param segment_max the total number of permissible segments  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_segment_max (gaspi_number_t *segment_max);

/*
 * FUNCTION:
 *   queue_num
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_NUM (queue_num)
 *
 * PARAMETER:
 *   @param queue_num the number of available queues  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_queue_num (gaspi_number_t *queue_num);

/*
 * FUNCTION:
 *   queue_size_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_SIZE_MAX ( queue_size_max )
 *
 * PARAMETER:
 *   @param queue_size_max the maximum number of simultaneous requests allowed 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_queue_size_max ( gaspi_number_t* queue_size_max );

/*
 * FUNCTION:
 *   queue_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_QUEUE_MAX ( queue_max )
 *
 * PARAMETER:
 *   @param queue_max the maximum number of allowed queues  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_queue_max ( gaspi_number_t *queue_max );

/*
 * FUNCTION:
 *   transfer_size_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_TRANSFER_SIZE_MAX (transfer_size_max)
 *
 * PARAMETER:
 *   @param transfer_size_max the maximum transfer size allowed for a single request 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_transfer_size_max (gaspi_size_t *transfer_size_max);

/*
 * FUNCTION:
 *   notification_num
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_NOTIFICATION_NUM (notification_num)
 *
 * PARAMETER:
 *   @param notification_num the number of available notifications  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_notification_num (gaspi_number_t *notification_num);

/*
 * FUNCTION:
 *   passive_transfer_size_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PASSIVE_TRANSFER_SIZE_MAX (transfer_size_max)
 *
 * PARAMETER:
 *   @param transfer_size_max maximal transfer size per single passive communication 
 *          request  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_passive_transfer_size_max (gaspi_size_t *transfer_size_max);

/*
 * FUNCTION:
 *   atomic_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ATOMIC_MAX (max_value)
 *
 * PARAMETER:
 *   @param max_value the maximum value an gaspi_atomic_value_t can hold 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_atomic_max (gaspi_atomic_value_t *max_value);

/*
 * FUNCTION:
 *   allreduce_buf_size
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ALLREDUCE_BUF_SIZE (buf_size)
 *
 * PARAMETER:
 *   @param buf_size the size of the internal buffer in gaspi_allreduce_user 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_allreduce_buf_size (gaspi_size_t *buf_size);

/*
 * FUNCTION:
 *   allreduce_elem_max
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_ALLREDUCE_ELEM_MAX (elem_max)
 *
 * PARAMETER:
 *   @param elem_max the maximum number of elements allowed in gaspi_allreduce 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_allreduce_elem_max (gaspi_number_t *elem_max);

/*
 * FUNCTION:
 *   network_type
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_NETWORK_TYPE (network_type)
 *
 * PARAMETER:
 *   @param network_type the chosen network type  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_network_type (gaspi_network_t *network_type);

/*
 * FUNCTION:
 *   build_infrastructure
 *
 * DESCRIPTION:
 *  
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_BUILD_INFRASTRUCTURE (build_infrastructure)
 *
 * PARAMETER:
 *   @param build_infrastructure the current value of build_infrastructure  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_build_infrastructure (gaspi_number_t *build_infrastructure);

/*
 * FUNCTION:
 *   version
 *
 * DESCRIPTION:
 *   The gaspi_version procedure is a synchronous local 
 *   blocking procedure which determines the version of the running 
 *   GASPI installation. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_VERSION (version)
 *
 * PARAMETER:
 *   @param version The version of the running GASPI installation  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_version (float *version);

/*
 * FUNCTION:
 *   time_get
 *
 * DESCRIPTION:
 *   The gaspi_time_get procedure is a synchronous local 
 *   blocking procedure which determines the time elapsed since an 
 *   arbitrary point of time in the past. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_TIME_GET (wtime)
 *
 * PARAMETER:
 *   @param wtime time elapsed in milliseconds  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_time_get (gaspi_time_t *wtime);

/*
 * FUNCTION:
 *   time_ticks
 *
 * DESCRIPTION:
 *   The gaspi_time_ticks procedure is a synchronous local 
 *   blocking procedure which returns the resolution of the internal 
 *   timer in terms of milliseconds. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_TIME_TICKS (resolution)
 *
 * PARAMETER:
 *   @param resolution the resolution of the internal timer in milliseconds 
 *           (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_time_ticks (gaspi_time_t *resolution);

/*
 * FUNCTION:
 *   print_error
 *
 * DESCRIPTION:
 *   The gaspi_print_error procedure is a synchronous local 
 *   blocking procedure which translates an error code to a text message. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PRINT_ERROR( error_code
 *                    , error_message )
 *
 * PARAMETER:
 *   @param error_code the error code to be translated  (in)
 *   @param error_message the error message  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_print_error( gaspi_return_t const error_code
                 , gaspi_string_t *error_message );

/*
 * FUNCTION:
 *   statistic_counter_max
 *
 * DESCRIPTION:
 *   The gaspi_statistic_counter_max procedure is a synchronous 
 *   local blocking procedure, which provides a way to inform the 
 *   GASPI user dynamically about the number of avialable counters. 
 *   An implementation should not provide a compile-time constant 
 *   maximum for statistic_counter. Instead the user can call gaspi_statistic_counter_max 
 *   in order to determine the maximum value for statistic_counter. 
 *   
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATISTIC_COUNTER_MAX ( counter_max )
 *
 * PARAMETER:
 *   @param counter_max the maximum value for statistic_counter. The allowed 
 *          value range is 0 <= counter < counter_max  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_statistic_counter_max ( gaspi_number_t *counter_max );

/*
 * FUNCTION:
 *   statistic_counter_info
 *
 * DESCRIPTION:
 *   The gaspi_statistic_counter_info procedure is a synchronous 
 *   local blocking procedure which provides an implementation independent 
 *   way to retrieve information for a particular statistic counter. 
 *   Beside the name and a description this function also yields the 
 *   meaning of the argument value for this counter, if any. The meaning 
 *   is defined in terms of the statistic_argument enumeration. 
 *
 *   
 *   typedef enum { GASPI_STATISTIC_ARGUMENT_NONE , GASPI_STATISTIC_ARGUMENT_RANK 
 *   , ... } gaspi_statistic_argument_t; 
 *   A GASPI implementation 
 *   is free to extend the above enumeration. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATISTIC_COUNTER_INFO ( counter
 *                                , argument
 *                                , counter_name
 *                                , counter_description
 *                                , verbosity_level )
 *
 * PARAMETER:
 *   @param counter the counter, for which detailed information is requested 
 *           (in)
 *   @param counter_argument the meaning of the argument value  (out)
 *   @param counter_name a short name of this counter  (out)
 *   @param counter_description a more verbose description of this counter  (out)
 *   @param verbosity_level minimum verbosity level to activate this counter (at 
 *          least 1)  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_statistic_counter_info ( gaspi_statistic_counter_t const counter
                             , gaspi_statistic_argument_t 
                                     *argument
                             , gaspi_string_t *counter_name
                             , gaspi_string_t 
                                     *counter_description
                             , gaspi_number_t *verbosity_level );

/*
 * FUNCTION:
 *   statistic_verbosity_level
 *
 * DESCRIPTION:
 *   The gaspi_statistic_verbosity_level procedure is a 
 *   synchronous local blocking procedure which sets the process-wide 
 *   verbosity level of the statistic interface. A counter is only 
 *   active (that is, it is updated), if the process-wide verbosity 
 *   level is higher or equal to the minimum verbosity level of that 
 *   counter. If a call to gaspi_statistic_verbosity_level 
 *   activates or deactivates counters and there are asynchronous 
 *   operations in progress, it is unspecified, whether and how these 
 *   counters are affected by the operations. It is furthermore unspecified 
 *   whether and how counters of higher verbositiy levels are updated. 
 *   A verbosity level of 0 deactivates all counting. It is not guaranteed, 
 *   that counters with a minimum verbosity level of 0 are counted 
 *   properly, if the verbosity level is set to 0. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATISTIC_VERBOSITY_LEVEL ( verbosity_level )
 *
 * PARAMETER:
 *   @param verbosity_level the level of desired verbosity  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_statistic_verbosity_level ( gaspi_number_t const verbosity_level);

/*
 * FUNCTION:
 *   statistic_counter_get
 *
 * DESCRIPTION:
 *   The gaspi_statistic_counter_get procedure is a synchronous 
 *   local blocking procedure which retrieves a statistical counter 
 *   from the local GASPI process. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATISTIC_COUNTER_GET ( counter
 *                               , argument
 *                               , value )
 *
 * PARAMETER:
 *   @param counter the counter to be retrieved  (in)
 *   @param argument the argument for the counter  (in)
 *   @param value the current value of the counter  (out)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_statistic_counter_get ( gaspi_statistic_counter_t const counter
                            , gaspi_statistic_argument_t const argument
                            , gaspi_number_t *value );

/*
 * FUNCTION:
 *   statistic_counter_reset
 *
 * DESCRIPTION:
 *   The gaspi_statistic_counter_reset procedure is a synchronous 
 *   local blocking procedure which sets a statistical counter to 
 *   0. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_STATISTIC_COUNTER_RESET (counter)
 *
 * PARAMETER:
 *   @param counter the counter to be reset  (in)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_statistic_counter_reset (gaspi_statistic_counter_t const counter);

/*
 * FUNCTION:
 *   pcontrol
 *
 * DESCRIPTION:
 *   The function gaspi_pcontrol is a no-op. A GASPI implementation 
 *   itself ignores the value of argument and returns immediately. 
 *   This routine is provided in order to enable users to communicate 
 *   with an event trace interface from inside the application. The 
 *   meaning of argument is specified by the used event tracer. 
 *
 * FUNCTION PROTOTYPE:
 *   GASPI_PCONTROL ( argument )
 *
 * PARAMETER:
 *   @param argument  (inout)
 *
 * RETURN VALUE:
 *   @return GASPI_SUCCESS in case of success.
 *           GASPI_ERROR in case of error.
 */

gaspi_return_t
gaspi_pcontrol ( gaspi_pointer_t argument );



#ifdef __cplusplus
}
#endif
#endif
