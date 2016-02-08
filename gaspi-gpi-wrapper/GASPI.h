#ifndef _GASPI_H
#define _GASPI_H 1
/********************************************************/
/*                                                      */
/*                                                      */
/*                                                      */
/*                 GASPI Interface                      */
/*                                                      */
/*                                                      */
/*                                                      */
/********************************************************/

#define GASPI_BLOCK 0
#define GASPI_GROUP_ALL 0
#define GASPI_NORANK 0
#define GASPI_NOQUEUESIZE (gaspi_number_t)(-1)
#define GASPI_NOSTRING (gaspi_string_t)(0)

/*!
\typedef
\brief The GASPI rank type.
\details
*/

typedef unsigned int gaspi_rank_t;

/*!
\typedef
\brief The GASPI memory segment ID type.
\details
*/

typedef unsigned short gaspi_segment_id_t;

/*!
\typedef
\brief The GASPI offset type. Offsets are measured relative to
\details
*/

typedef unsigned long gaspi_offset_t;

/*!
\typedef
\brief The GASPI size type. Sizes are measured in units of bytes.
\details
*/

typedef unsigned long gaspi_size_t;

/*!
\typedef
\brief  The GASPI queue ID type.
\details
*/

typedef unsigned short gaspi_queue_id_t;

/*!
\typedef
\brief The GASPI notification ID type.
\details
*/

typedef unsigned short gaspi_notification_id_t;

/*!
\typedef
\brief The GASPI notification value type.
\details
*/

typedef int gaspi_notification_t;

/*!
\typedef
\brief The GASPI Tag type. Tag are used to discriminate
\details
*/

typedef int gaspi_tag_t;

/*!
\typedef
\brief The GASPI global atomic counter ID type.
\details
*/

typedef unsigned short gaspi_counter_id_t;

/*!
\typedef
\brief The GASPI global atomic counter value type.
\details
*/

typedef long gaspi_counter_value_t;

/*!
\typedef
\brief The GASPI return value type.
\details
*/

typedef enum
  {
    GASPI_ERROR = -1
    , GASPI_SUCCESS = 0
    , GASPI_TIMEOUT =1
  }
  gaspi_return_t;

/*!
\typedef
\brief The vector with return codes for individual nodes. The
\details
*/

typedef gaspi_return_t* gaspi_returns_t;

/*!
\typedef
\brief The GASPI timeout type.
\details
*/

typedef int gaspi_timeout_t;

/*!
\typedef
\brief A type that is used to count elements. That could be numbers
\details
*/

typedef unsigned int gaspi_number_t;

/*!
\typedef
\brief the GASPI group type
\details
*/

typedef unsigned short gaspi_group_t;


/*!
\typedef
\brief A type that can point to some memory.
\details
*/

typedef void* gaspi_pointer_t;

/*!
\typedef
\brief the GASPI network infrastructure type
\details
*/

typedef unsigned int gaspi_network_t;

/*!
\typedef
\brief the GASPI boolean type
\details
*/

typedef int gaspi_bool_t;

/*!
\typedef
\brief the GASPI string type
\details
*/

typedef char* gaspi_string_t;

/*!
\typedef
\brief the GASPI configuration object
\details
*/

typedef struct {

  gaspi_segment_id_t segment_num;


  gaspi_number_t     group_num;

  gaspi_number_t     queue_num;
  gaspi_number_t     queue_size_max;
  gaspi_size_t       message_size_max;
  
  gaspi_notification_id_t    notify_flag_num;

  gaspi_number_t     passive_queue_size_max;
  gaspi_size_t       passive_message_size_max;

  gaspi_number_t     counter_num;

  gaspi_network_t    network;

  gaspi_bool_t       param_check;

  void *             user_defined;

} gaspi_configuration_t;

gaspi_configuration_t GASPI_CONFIGURATION_DEFAULT_VALUE (void);

#define GASPI_CONFIGURATION_DEFAULT GASPI_CONFIGURATION_DEFAULT_VALUE()

/*!


\param [in] configuration  the configuration object with the desired
\param [in] timeout  the timeout.
*/

gaspi_return_t
gaspi_proc_init ( gaspi_configuration_t configuration, gaspi_timeout_t timeout ); 

/*!


\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_proc_term (gaspi_timeout_t timeout );

/*!


\param [in] rank  the rank of the process to be killed
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_proc_kill ( gaspi_rank_t rank
                , gaspi_timeout_t timeout );

/*!


\param [out] rank  the rank of the calling \GASPI   process.
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_proc_rank (gaspi_rank_t* rank);

/*!


\param [out] proc\_num  the total number of \GASPI   processes
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_proc_num ( gaspi_rank_t* proc_num );


/*!
\typedef
\brief The state of a remove GASPI process
\details
*/

typedef enum
  {
    GASPI_STATE_HEALTHY = 0
    , GASPI_STATE_CORRUPT =1
  }
  gaspi_state_t;

/*!
\typedef
\brief The vector with state information for individual nodes. The
\details
*/

typedef gaspi_state_t* gaspi_state_vector_t;

/*!


\param [out] returns  the vector with individual return codes
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_state_vec_get( gaspi_state_vector_t state_vector );

/*!


\param  inout  state\_vector  the vector with individual return codes
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_state_vec_reset( gaspi_state_vector_t state_vector ) ;

/*!


\param [out] group  the created empty group
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_create (gaspi_group_t group);


/*!


\param  inout  group  the group to add the rank to
\param [in] rank  the absolute node rank to add to the group
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_add ( gaspi_group_t group
                , gaspi_rank_t rank
                );


/*!


\param [in] group  the group to commit
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_commit ( gaspi_group_t group
                   , gaspi_timeout_t timeout
                   );


/*!


\param [in] group  the group to be deleted
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_delete ( gaspi_group_t group );


/*!


\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_max (gaspi_number_t group_max);


/*!


\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_size ( gaspi_group_t group
                 , gaspi_number_t group_size );


/*!


\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_group_ranks( gaspi_group_t group
		   , gaspi_rank_t  group_ranks
		   , gaspi_number_t group_size );


/*!


\param [in] segment\_id   The segment ID to be created. The segment ID's need to be
\param [in] size  The size of the segment in bytes
\param  opt  alloc\_policy  optional parameter characterizing the allocation policy
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_alloc ( gaspi_segment_id_t segment_id
                    , gaspi_size_t size
                    , gaspi_number_t alloc_policy
                    );


/*!


\param [in] segment\_id   The segment ID to be registered. The segment ID's need to be
\param [in] rank  The rank of the \GASPI   process which should register the new segment
\param [in] timeout  The timeout for the operation
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_register ( gaspi_segment_id_t segment_id
                       , gaspi_rank_t rank
                       , gaspi_timeout_t timeout
                       );


/*!


\param [in] segment\_id  The segment ID to be created. The segment ID's need to be
\param [in] size  The size of the segment in bytes
\param [in] group  The group which should create the segment
\param [in] timeout  The timeout for the operation
\param  opt  alloc\_policy  optional parameter characterizing the allocation policy
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_create ( gaspi_segment_id_t segment_id
                     , gaspi_size_t size
                     , gaspi_group_t group
                     , gaspi_timeout_t timeout
                     /*                     , gaspi_number_t alloc_policy */
                     );


/*!


\param [in] segment\_id   The segment ID to be freed.
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_free ( gaspi_segment_id_t segment_id );


/*!


\param [in] segment\_id  The segment ID.
\param [out] pointer  The pointer to the memory segment.
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_ptr ( gaspi_segment_id_t segment_id
                  , gaspi_pointer_t* pointer
                  );


/*!


\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_segment_max (gaspi_segment_id_t segment_max);


/*!


\param [in] segment\_id\_local  the local segment ID to be written from
\param [in] offset\_local  the local offset in bytes to be written from
\param [in] rank  the remote rank to be written to
\param [in] segment\_id\_remote  the remote segment to be written to
\param [in] offset\_remote  the remote offset to be written to
\param [in] size  the size of the message to be sent
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_id_t queue
            , gaspi_timeout_t timeout
            );


/*!


\param [in] segment\_id\_local  the local segment ID to be read to
\param [in] offset\_local  the local offset in bytes to be read to
\param [in] rank  the remote rank to be read from
\param [in] segment\_id\_remote  the remote segment to be read from
\param [in] offset\_remote  the remote offset to be read from
\param [in] size  the size of the message to be read
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           );


/*!


\param [in] queue  the queue ID to wait for
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_wait ( gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           );


/*!


\param [in] rank  the remote rank to notify
\param [in] flag\_id  the remote flag ID where the flag is sent to
\param [in] flag_value  the flag to be sent
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_notify ( gaspi_rank_t rank
             , gaspi_notification_id_t flag_id
             , gaspi_notification_t flag_value
             , gaspi_queue_id_t queue
             , gaspi_timeout_t timeout
             );


/*!


\param [in] flag\_id  the local flag ID where the flag to compare to resides
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_notify_wait ( gaspi_notification_id_t flag_id
                  , gaspi_timeout_t timeout
                  );

gaspi_return_t
gaspi_notify_waitsome ( gaspi_notification_id_t flag_id_beg
                      , gaspi_notification_id_t flag_num
                      , gaspi_timeout_t timeout
                      );


/*!


\param [in] flag\_id  the local flag ID to reset
\param [out] flag\_val  old flag value
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_notify_reset ( gaspi_notification_id_t flag_id
                   , gaspi_notification_t* flag_val
                   );


/*!


\param [out] flag\_max  number of flags
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_notification_num ( gaspi_notification_id_t* flag_max );


/*!


\param [in] segment\_id\_local  the local segment ID to be written from
\param [in] offset\_local  the local offset in bytes to be written from
\param [in] rank  the remote rank to be written to
\param [in] segment\_id\_remote  the remote segment to be written to
\param [in] offset\_remote  the remote offset to be written to
\param [in] flag_id  the remote segment ID where the flag is sent to
\param [in] flag  the flag to be sent
\param [in] size  the size of the message to be sent
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_write_notify ( gaspi_segment_id_t segment_id_local
                   , gaspi_offset_t offset_local
                   , gaspi_rank_t rank
                   , gaspi_segment_id_t segment_id_remote
                   , gaspi_offset_t offset_remote
                   , gaspi_size_t size
                   , gaspi_notification_id_t flag_id
                   , gaspi_notification_t flag_value
                   , gaspi_queue_id_t queue
                   , gaspi_timeout_t timeout
                   );


/*!


\param [in] num  the number of elements to be sent
\param [in] segment\_id\_local[num]  list of local segment IDs to be written from
\param [in] offset\_local[num]  list of local offsets in bytes to be written from
\param [in] rank  the remote rank to be written to
\param [in] segment\_id\_remote[num]  list of remote segments to be written to
\param [in] offset\_remote[num]  list of remote offsets to be written to
\param [in] size[num]  list of sizes of the messages to be sent
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_write_list ( gaspi_number_t num
                 , gaspi_segment_id_t* segment_id_local
                 , gaspi_offset_t* offset_local
                 , gaspi_rank_t rank
                 , gaspi_segment_id_t* segment_id_remote
                 , gaspi_offset_t* offset_remote
                 , gaspi_size_t* size
                 , gaspi_queue_id_t queue
                 , gaspi_timeout_t timeout
                 );


/*!


\param [in] num  the number of elements to be sent
\param [in] segment\_id\_local[num]  list of local segment IDs to be written from
\param [in] offset\_local[num]  list of local offsets in bytes to be written from
\param [in] rank  the remote rank to be written to
\param [in] segment\_id\_remote[num]  list of remote segments to be written to
\param [in] offset\_remote[num]  list of remote offsets to be written to
\param [in] size[num]  list of sizes of the messages to be sent
\param [in] flag_id  the remote flag ID where the flag is sent to
\param [in] flag_value  the flag to be sent
\param [in] offset\_flag  the remote offset where the flag is sent to
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_write_list_notify ( gaspi_number_t num
                        , gaspi_segment_id_t* segment_id_local
                        , gaspi_offset_t* offset_local
                        , gaspi_rank_t rank
                        , gaspi_segment_id_t* segment_id_remote
                        , gaspi_offset_t* offset_remote
                        , gaspi_size_t* size
                        , gaspi_notification_id_t flag_id
                        , gaspi_notification_t flag_value
                        , gaspi_offset_t offset_flag
                        , gaspi_queue_id_t queue
                        , gaspi_timeout_t timeout
                        );


/*!


\param [in] num  the number of elements to be read
\param [in] segment\_id\_local[num]  list of local segment IDs to be read to
\param [in] offset\_local[num]  list of local offsets in bytes to be read to
\param [in] rank  the remote rank to be read from
\param [in] segment\_id\_remote[num]  list of remote segments to be read from
\param [in] offset\_remote[num]  list of remote offsets to be read from
\param [in] size[num]  list of sizes of the messages to be read
\param [in] queue  the queue to be used
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_read_list ( gaspi_number_t num
                , gaspi_segment_id_t* segment_id_local
                , gaspi_offset_t* offset_local
                , gaspi_rank_t rank
                , gaspi_segment_id_t* segment_id_remote
                , gaspi_offset_t* offset_remote
                , gaspi_size_t* size
                , gaspi_queue_id_t queue
                , gaspi_timeout_t timeout
                );


/*!


\param [out] queue\_max  the maximum number of available queues
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_queue_num (gaspi_queue_id_t queue_max );


/*!


\param [in] queue  the queue to probe
\param [out] queue\_size  the number of requests posted to the queue
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_queue_size ( gaspi_queue_id_t queue
                 , gaspi_number_t* queue_size
                 );


/*!


\param [in] queue  the queue to probe
\param [out] queue\_size  the number of simultaneously allowed requests
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_queue_size_max ( gaspi_queue_id_t queue
                     , gaspi_number_t* queue_size
                     );


/*!


\param [out] message\_size  the maximally allowed message size for a single request
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_message_size_max ( gaspi_size_t message_size );


/*!


\param [in] segment\_id\_local  the local segment ID where the message is send from
\param [in] offset\_local  the local offset where the message is sent from
\param [in] rank  the remote Rank where the message is sent to
\param [in] size  the size of the message to be sent to
\param [in] tag  the tag to be sent with the message
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_send ( gaspi_segment_id_t segment_id_local
                   , gaspi_offset_t offset_local
                   , gaspi_rank_t rank
                   , gaspi_size_t size
                   , gaspi_tag_t tag
                   , gaspi_timeout_t timeout
                   );


/*!


\param [in] Timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_wait (gaspi_timeout_t timeout);


/*!


\param [in] segment\_id\_local  the local segment ID where the message is written to
\param [in] offset\_local  the local offset where the message is written to
\param [out] rank  the remote rank from which the message is received
\param [out] tag  the tag which has been sent with the message
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_receive ( gaspi_segment_id_t segment_id_local
                      , gaspi_offset_t offset_local
                      , gaspi_size_t size
                      , gaspi_rank_t rank
                      , gaspi_tag_t tag
                      , gaspi_timeout_t timeout
                      );


/*!


\param [out] queue\_size  Number of requests posted to the passive communication queue
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_queue_size (gaspi_number_t *queue_size);


/*!


\param [out] queue\_size  Maximally available number of requests in the passive communication queue
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_queue_size_max  (gaspi_number_t *queue_size);


/*!


\param [out] message\_size  maximal message size per single passive communication request
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_passive_message_size_max (gaspi_size_t message_size);


typedef unsigned int gaspi_statistic_counter_t;

typedef enum{GASPI_STATISTIC_ARGUMENT_NONE
			, GASPI_STATISTIC_ARGUMENT_RANK
			, GASPI_STATISTIC_ARGUMENT_FUNC
		}gaspi_statistic_argument_t;

typedef unsigned int gaspi_argument_t;

/*gaspi_return_t
gaspi_statistic_counter_get ( gaspi_statistic_counter_t counter
			, gaspi_statistic_value_t value
			);*/
			
gaspi_return_t 
gaspi_statistic_verbosity_level(gaspi_number_t _verbosity_level);
			
gaspi_return_t 
gaspi_statistic_counter_max(gaspi_statistic_counter_t* counter_max);

gaspi_return_t 
gaspi_statistic_counter_info(gaspi_statistic_counter_t counter
			, gaspi_statistic_argument_t* counter_argument
			, gaspi_string_t* counter_name
			, gaspi_string_t* counter_description
			, gaspi_number_t* verbosity_level
			);
			
gaspi_return_t 
gaspi_statistic_counter_get ( gaspi_statistic_counter_t counter
			, gaspi_number_t argument
			, gaspi_number_t* value
			);
gaspi_return_t 
gaspi_statistic_counter_reset (gaspi_statistic_counter_t counter);

/*gaspi_return_t
gaspi_statistic_counter_collect (gaspi_statistic_counter_t counter
			,gaspi_statistic_value_t value
			,gaspi_group_t group
			,gaspi_timeout_t timeout
			);*/

/*!


\param [in] counter\_id  the counter to be reseted
\param [in] value  the value to which the global atomic counter is reseted
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_counter_reset ( gaspi_counter_id_t counter_id
                    , gaspi_counter_value_t value
                    , gaspi_timeout_t timeout
                    );


/*!


\param [in] counter\_id  the global atomic counter to be used for the operation
\param [in] value\_add  the value which is to be added to the global atomic counter
\param [out] value\_old  the value of the counter before the operation
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_counter_fetch_add ( gaspi_counter_id_t counter_id
                        , gaspi_counter_value_t value_add
                        , gaspi_counter_value_t value_old
                        , gaspi_timeout_t timeout
                        );


/*!


\param [in] counter\_id  the global atomic counter to be used for the operation
\param [in] comparator  the value which is compared to the value of the counter
\param [in] value\_new  the new value to which the counter is set if the comparison is true
\param [out] value\_old  the value of the counter before the operation
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_counter_compare_swap ( gaspi_counter_id_t counter_id
                           , gaspi_counter_value_t comparator
                           , gaspi_counter_value_t value_new
                           , gaspi_counter_value_t value_old
                           , gaspi_timeout_t timeout
                           );


/*!


\param [out] counter\_max  The number of available global atomic counters
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_counter_num (gaspi_number_t counter_max);



typedef enum { GASPI_OP_MIN
             , GASPI_OP_MAX
             , GASPI_OP_SUM
             } gaspi_operation_t;


typedef enum { GASPI_TYPE_INT
             , GASPI_TYPE_UINT
             , GASPI_TYPE_LONG
             , GASPI_TYPE_ULONG
             , GASPI_TYPE_FLOAT
             , GASPI_TYPE_DOUBLE
             } gaspi_datatype_t;


/*!


\param [in] group  the group of ranks which should participate in the barrier
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_barrier ( gaspi_group_t group
              , gaspi_timeout_t timeout
              );


/*!


\param [in] buffer\_send  pointer to the buffer where the input is placed
\param [in] buffer\_receive  pointer to the buffer where the result is placed
\param [in] num  the number of elements to be reduced on each process
\param [in] operation  the \GASPI   reduction operation type
\param [in] datatype  the \GASPI   element type
\param [in] group  the group of ranks which participate in the reduction operation
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_allreduce ( gaspi_pointer_t buffer_send
                , gaspi_pointer_t buffer_receive
                , unsigned char num
                , gaspi_operation_t operation
                , gaspi_datatype_t datatype
                , gaspi_group_t group
                , gaspi_timeout_t timeout
                );


/*!


\param [out] elem\_max  the maximum number of allowed elements in \gaspifunction allreduce
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_allreduce_elem_max (gaspi_number_t elem_max);



/*!
\typedef
\brief The GASPI reduction operation type
\details
*/

typedef void* gaspi_reduce_operation_t;

/*!
\typedef
\brief The GASPI reduction operation state type
\details
*/

typedef void* gaspi_reduce_state_t;


/*!


\param [in] buffer\_send  pointer to the buffer where the input is placed
\param [in] buffer\_receive  pointer to the buffer where the result is placed
\param [in] num  the number of elements to be reduced on each process
\param [in] size\_element  Size in bytes of one element to be reduced
\param [in] reduce\_operation  pointer to the user defined reduction operation procedure
\param  in,out  reduce\_state  reduction state vector
\param [in] group  the group of ranks which participate in the reduction operation
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_allreduce_user ( gaspi_pointer_t buffer_send
                     , gaspi_pointer_t buffer_receive
                     , unsigned char num
                     , gaspi_size_t size_element
                     , gaspi_reduce_operation_t reduce_operation
                     , gaspi_reduce_state_t reduce_state
                     , gaspi_group_t group
                     , gaspi_timeout_t timeout
                     );


/*!


\param [in] operand\_one  pointer to the first operand
\param [in] operand\_two  pointer to the second operand
\param [in] result  pointer to the result
\param [in] state  pointer to the state
\param [in] timeout  the timeout
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_reduce_operation ( gaspi_pointer_t operand_one
                       , gaspi_pointer_t operand_two
                       , gaspi_pointer_t result
                       , gaspi_state_t state
                       , gaspi_timeout_t timeout
                       );



/*!


\param [out] buf\_size  the size of the internal buffer in \gaspifunction allreduce\_user
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_allreduce_buf_size (gaspi_number_t buf_size);


/*!


\param [out] version  The version of the running \GASPI   installation
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_version (float version);


/*!
\typedef
\brief Time
\details
*/

typedef unsigned int gaspi_time_t;


/*!


\param [out] wtime  time elapsed in miliseconds
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_time_get ( gaspi_time_t* wtime );


/*!


\param [out] resolution  the resolution of the internal timer in miliseconds
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_time_ticks ( gaspi_time_t resolution );


/*!


\param [in] error\_code  the error code to be translated
\param [out] error\_message  the error message
\return gaspi_return_t
\retval GASPI_SUCCESS: operation has successfully returned
\retval GASPI_TIMEOUT: operation has run into timeout
\retval GASPI_ERROR: operation has terminated with an error*/

gaspi_return_t
gaspi_error_message ( gaspi_return_t error_code
                  , gaspi_string_t* error_message );

#endif
