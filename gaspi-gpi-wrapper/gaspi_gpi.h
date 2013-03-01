#ifndef GASPI_GPI_H
#define GASPI_GPI_H
#include <GASPI.h>

typedef struct {
  int argc;
  char ** argv;
}argument_t;

extern void gaspi_printf(char * format, ... );

// strong symbols
extern gaspi_return_t pgaspi_proc_rank(gaspi_rank_t *rank);
extern gaspi_return_t pgaspi_proc_init( gaspi_configuration_t configuration, gaspi_timeout_t timeout);
extern gaspi_return_t pgaspi_proc_term(gaspi_timeout_t timeout );
extern gaspi_return_t pgaspi_wait (gaspi_queue_id_t queue, gaspi_timeout_t timeout);
extern gaspi_return_t pgaspi_proc_rank (gaspi_rank_t *rank);
extern gaspi_return_t pgaspi_proc_num ( gaspi_rank_t *proc_num );
extern gaspi_return_t pgaspi_barrier ( gaspi_group_t group, gaspi_timeout_t timeout);
extern gaspi_return_t pgaspi_write ( gaspi_segment_id_t segment_id_local
            , gaspi_offset_t offset_local
            , gaspi_rank_t rank
            , gaspi_segment_id_t segment_id_remote
            , gaspi_offset_t offset_remote
            , gaspi_size_t size
            , gaspi_queue_id_t queue
            , gaspi_timeout_t timeout
            );
extern gaspi_return_t pgaspi_read ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_segment_id_t segment_id_remote
           , gaspi_offset_t offset_remote
           , gaspi_size_t size
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           );
extern gaspi_return_t pgaspi_write_notify ( gaspi_segment_id_t segment_id_local
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
extern gaspi_return_t pgaspi_notify ( gaspi_rank_t rank
           , gaspi_notification_id_t flag_id
           , gaspi_notification_t flag_value
           , gaspi_queue_id_t queue
           , gaspi_timeout_t timeout
           );
extern gaspi_return_t pgaspi_notify_waitsome ( gaspi_notification_id_t flag_id_beg
           , gaspi_notification_id_t flag_num
           , gaspi_timeout_t timeout
           );
extern gaspi_return_t pgaspi_notify_reset ( gaspi_notification_id_t flag_id
           , gaspi_notification_t* flag_val
           );
extern gaspi_return_t pgaspi_notification_num ( gaspi_notification_id_t* flag_max );
extern gaspi_return_t pgaspi_segment_ptr ( gaspi_segment_id_t segment_id
           , gaspi_pointer_t* pointer
           );
extern gaspi_return_t pgaspi_allreduce ( gaspi_pointer_t buffer_send
           , gaspi_pointer_t buffer_receive
           , unsigned char num
           , gaspi_operation_t operation
           , gaspi_datatype_t datatype
           , gaspi_group_t group
           , gaspi_timeout_t timeout
           );
extern gaspi_return_t pgaspi_passive_send ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_rank_t rank
           , gaspi_size_t size
           , gaspi_tag_t tag
           , gaspi_timeout_t timeout
           );
extern gaspi_return_t pgaspi_passive_wait (gaspi_timeout_t timeout);                   
extern gaspi_return_t pgaspi_passive_receive ( gaspi_segment_id_t segment_id_local
           , gaspi_offset_t offset_local
           , gaspi_size_t size
           , gaspi_rank_t* rank
           , gaspi_tag_t* tag
           , gaspi_timeout_t timeout
           );

#endif
