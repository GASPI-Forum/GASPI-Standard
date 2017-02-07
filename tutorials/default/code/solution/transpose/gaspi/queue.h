#ifndef QUEUE_H
#define QUEUE_H

#include <GASPI.h>
#include "assert.h"

#define WAIT_IF_QUEUE_FULL(f, queue)                    \
  {                                                     \
    gaspi_return_t ret;                                 \
    while ((ret = (f)) == GASPI_QUEUE_FULL)             \
      {                                                 \
        /* wait and re-submit */			\
	ASSERT (gaspi_wait ((queue), GASPI_BLOCK));	\
      }                                                 \
    ASSERT (ret == GASPI_SUCCESS);                      \
  }


#define FLIP_IF_QUEUE_FULL(f, queue)                    \
  {                                                     \
    gaspi_return_t ret;                                 \
    while ((ret = (f)) == GASPI_QUEUE_FULL)             \
      {							\
	/* flip queue, wait and re-submit */		\
	queue = 1 - (queue);				\
	ASSERT (gaspi_wait ((queue), GASPI_BLOCK));	\
      }                                                 \
    ASSERT (ret == GASPI_SUCCESS);                      \
  }



#endif
