#ifndef _WAIT_IF_QUEUE_FULL_H
#define _WAIT_IF_QUEUE_FULL_H 1

#include <GASPI.h>

void wait_if_queue_full ( const gaspi_queue_id_t queue_id
                        , const gaspi_number_t   request_size );

#endif
