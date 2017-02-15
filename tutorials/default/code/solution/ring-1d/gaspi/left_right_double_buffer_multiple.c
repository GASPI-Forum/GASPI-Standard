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

#include "gaspi/queue.h"
#include "gaspi/success_or_die.h"
#include "gaspi/waitsome.h"

#include "assert.h"
#include "constant.h"
#include "data.h"
#include "topology.h"
#include "now.h"

#include <GASPI.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{

  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t iProc, nProc;
  SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProc));

  // number of threads
  const int NTHREADS = 2;

  // number of buffers
  const int NWAY     = 2;

  gaspi_segment_id_t const segment_id = 0;

  // allocate segment for array for local vector, left halo and right halo
  SUCCESS_OR_DIE ( gaspi_segment_create
      ( segment_id, NWAY * (NTHREADS + 2) * 2 * VLEN * sizeof (double)
      , GASPI_GROUP_ALL, GASPI_BLOCK, GASPI_MEM_UNINITIALIZED));
  gaspi_pointer_t array;
  SUCCESS_OR_DIE ( gaspi_segment_ptr ( segment_id, &array) );

  // initial buffer id
  int buffer_id = 0;

  // set notification values
  gaspi_notification_id_t left_data_available[NWAY];
  gaspi_notification_id_t right_data_available[NWAY];
  for (gaspi_notification_id_t id = 0; id < NWAY; ++id)
  {
    left_data_available[id] = id;
    right_data_available[id] = NWAY + id;
  }

  // initialize data
  data_init (NTHREADS, iProc, buffer_id, array);

  omp_set_num_threads (NTHREADS);

  double time = -now();

#pragma omp parallel default (shared) firstprivate (buffer_id)
  {

    const int tid = omp_get_thread_num();

    for (int k = 0; k < NITER; ++k)
    {
      for ( int i = 0; i < nProc * NTHREADS; ++i )
      {

	const int left_halo   = 0;
	const int slice_id    = tid + 1;
	const int right_halo  = NTHREADS+1;
	
        if (tid == 0)
        {
	  // issue write
	  write_notify_and_cycle
	    ( segment_id
	      , array_OFFSET_left (buffer_id, left_halo + 1, 0)
	      , LEFT(iProc, nProc) 
	      , segment_id
	      , array_OFFSET_left (buffer_id, right_halo, 0)
	      , VLEN * sizeof (double)
	      , right_data_available[buffer_id]
	      , 1 + i
	      );

	  // wait for data notification
          wait_or_die (segment_id, left_data_available[buffer_id], 1 + i);

	  // compute data, read from id "buffer_id", write to id "1 - buffer_id"
          data_compute ( NTHREADS, array, 1 - buffer_id, buffer_id, slice_id);

        }
        if (tid == NTHREADS - 1)
	{
	  // issue write
	  write_notify_and_cycle
	    ( segment_id
	      , array_OFFSET_right (buffer_id, right_halo - 1, 0)
	      , RIGHT(iProc, nProc)
	      , segment_id
	      , array_OFFSET_right (buffer_id, left_halo, 0)
	      , VLEN * sizeof (double)
	      , left_data_available[buffer_id]
	      , 1 + i
	      );
	  
	  // wait for data notification
          wait_or_die (segment_id, right_data_available[buffer_id], 1 + i);

	  // compute data, read from id "buffer_id", write to id "1 - buffer_id"
          data_compute ( NTHREADS, array, 1 - buffer_id, buffer_id, slice_id);

	}
	if (tid > 0 && tid < NTHREADS - 1)
        {
	  // compute data, read from id "buffer_id", write to id "1 - buffer_id"
          data_compute ( NTHREADS, array, 1 - buffer_id, buffer_id, slice_id);
        }

#pragma omp barrier
	// alternate the buffer
	buffer_id = 1 - buffer_id;

      }
    }
  }

  time += now();

  data_verify (NTHREADS, iProc, (NITER * nProc * NTHREADS) % NWAY, array);

  printf ("# gaspi %s nProc %d vlen %i niter %d nthreads %i nway %i time %g\n"
         , argv[0], nProc, VLEN, NITER, NTHREADS, NWAY, time
         );

  gaspi_proc_term (GASPI_BLOCK);

  return EXIT_SUCCESS;
}
