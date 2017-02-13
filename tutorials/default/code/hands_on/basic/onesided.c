/*
 * This file is part of a small series of tutorial,
 * which aims to demonstrate key features of the GASPI
 * standard by means of small but expandable examples.
 * Conceptually the tutorial follows a MPI course
 * developed by EPCC and HLRS.
 *
 * Contact point for the MPI tutorial:
 *                 rabenseifner@hlrs.de
 * Contact point for the GASPI tutorial:
 *                 daniel.gruenewald@itwm.fraunhofer.de
 *                 mirko.rahn@itwm.fraunhofer.de
 *                 christian.simmendinger@t-systems.com
 */

#include "left_right.h"
#include "queue.h"
#include "success_or_die.h"
#include "waitsome.h"
#include "assert.h"

#include <GASPI.h>
#include <stdlib.h>
#include <stdio.h>

int
main(int argc,
     char *argv[])
{
  static const int VLEN = 1 << 2;

  SUCCESS_OR_DIE(gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t iProc, nProc;
  SUCCESS_OR_DIE(gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE(gaspi_proc_num (&nProc));

  gaspi_segment_id_t const segment_id = 0;
  gaspi_size_t const segment_size = 2 * VLEN * sizeof(double);

  SUCCESS_OR_DIE(gaspi_segment_create
		 (segment_id,
		  segment_size,
		  GASPI_GROUP_ALL,
		  GASPI_BLOCK,
		  GASPI_MEM_UNINITIALIZED
		  ));

  gaspi_pointer_t array;
  SUCCESS_OR_DIE(gaspi_segment_ptr (segment_id,
				    &array));

  double * src_array = (double *) (array);
  double * rcv_array = src_array + VLEN;

  for (int j = 0; j < VLEN; ++j)
    {
      src_array[j] = (double) (iProc * VLEN + j);
    }

  gaspi_notification_id_t data_available = 0;
  gaspi_queue_id_t queue_id = 0;

  gaspi_offset_t loc_off = 0;
  gaspi_offset_t rem_off = VLEN * sizeof(double);

  /*
   TODO
   ====
   - check queue.
   - gaspi_write_notify 1-dim array of size VLEN to right neighbour
   - wait for notification from left neighbour
   */

  for (int j = 0; j < VLEN; ++j)
    {
      printf ("rank %d rcv elem %d: %f \n",
	      iProc,
	      j,
	      rcv_array[j]);
    }

  /*
   TODO
   ====
   - why do we have to wait for the queue here ?
   - use the write_notify_and_cycle function in queue.c
  */

  SUCCESS_OR_DIE(gaspi_wait (queue_id,
			     GASPI_BLOCK));

  SUCCESS_OR_DIE(gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}
