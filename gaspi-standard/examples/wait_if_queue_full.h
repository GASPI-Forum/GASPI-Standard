#ifndef _WAIT_IF_QUEUE_FULL_H
#define _WAIT_IF_QUEUE_FULL_H 1

#include <GASPI.h>

#define WAIT_IF_QUEUE_FULL(f, queue)                    \
  {                                                     \
    gaspi_return_t ret;                                 \
    while ((ret = (f)) == GASPI_QUEUE_FULL)             \
       {                                                \
          ASSERT (gaspi_wait ((queue), GASPI_BLOCK));	\
       }                                                \
    ASSERT (ret == GASPI_SUCCESS);                      \
  }
#endif
