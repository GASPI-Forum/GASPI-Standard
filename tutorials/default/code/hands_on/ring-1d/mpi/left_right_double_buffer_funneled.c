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

#include "assert.h"
#include "constant.h"
#include "data.h"
#include "topology.h"
#include "now.h"

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DATAKEY_LEFT 4712
#define DATAKEY_RIGHT 4713

int
main(int argc,
     char *argv[])
{

  MPI_Init (&argc,
	    &argv);

  int nProc, iProc;

  MPI_Comm_rank (MPI_COMM_WORLD,
		 &iProc);
  MPI_Comm_size (MPI_COMM_WORLD,
		 &nProc);

  // number of threads
  const int NTHREADS = 2;

  // number of buffers
  const int NWAY = 2;

  // left neighbour
  const int left = LEFT(iProc,
			nProc);

  // right neighbour
  const int right = RIGHT(iProc,
			  nProc);

  // allocate array of for local vector, left halo and right halo
  double* array = malloc (NWAY * (NTHREADS + 2) * 2 * VLEN * sizeof(double));
  ASSERT (array != 0);


  // initialize data
  data_init (NTHREADS,
	     iProc,
	     0,
	     array);

  omp_set_num_threads (NTHREADS);

  MPI_Barrier (MPI_COMM_WORLD);

  double time = -now ();

#pragma omp parallel default (none) shared(array, nProc, iProc, NTHREADS, NWAY,  left, right)
    {
      // initial buffer id
      int buffer_id = 0;
      const int tid = omp_get_thread_num ();

      for (int k = 0; k < NITER; ++k)
        {
	  for (int i = 0; i < nProc * NTHREADS; ++i)
	    {

	      const int slice_id = tid + 1;
	      const int left_halo = 0;
	      const int right_halo = NTHREADS + 1;

	      if (tid == 0)
	        {
		  MPI_Request send_req[2];
		  MPI_Request recv_req[2];

		  // post recv
		  MPI_Irecv (&array_ELEM_right(buffer_id,
					       left_halo,
					       0),
			     VLEN,
			     MPI_DOUBLE,
			     left,
			     DATAKEY_RIGHT,
			     MPI_COMM_WORLD,
			     &recv_req[0]);

		  // post recv
		  MPI_Irecv (&array_ELEM_left(buffer_id,
					      right_halo,
					      0),
			     VLEN,
			     MPI_DOUBLE,
			     right,
			     DATAKEY_LEFT,
			     MPI_COMM_WORLD,
			     &recv_req[1]);

		  // issue send
		  MPI_Isend (&array_ELEM_right(buffer_id,
					       right_halo - 1,
					       0),
			     VLEN,
			     MPI_DOUBLE,
			     right,
			     DATAKEY_RIGHT,
			     MPI_COMM_WORLD,
			     &send_req[0]);

		  // issue send
		  MPI_Isend (&array_ELEM_left(buffer_id,
					      left_halo + 1,
					      0),
			     VLEN,
			     MPI_DOUBLE,
			     left,
			     DATAKEY_LEFT,
			     MPI_COMM_WORLD,
			     &send_req[1]);

		  // free send request
		  MPI_Request_free (&send_req[0]);
		  MPI_Request_free (&send_req[1]);

		  // wait for Irecv, Isend
		  MPI_Waitall (2,
			       recv_req,
			       MPI_STATUSES_IGNORE);

		}

#pragma omp barrier

	      // compute data, read from id "buffer_id", write to id "1 - buffer_id"
	      data_compute (NTHREADS,
			    array,
			    1 - buffer_id,
			    buffer_id,
			    slice_id);

#pragma omp barrier

	      // alternate the buffer
	      buffer_id = 1 - buffer_id;

	    }
	}
    }
  time += now ();

  data_verify (NTHREADS,
	       iProc,
	       (NITER * nProc * NTHREADS) % NWAY,
	       array);

  printf ("# mpi %s nProc %d vlen %i niter %d nthreads %i nway %i time %g\n",
	  argv[0],
	  nProc,
	  VLEN,
	  NITER,
	  NTHREADS,
	  NWAY,
	  time
	  );

  MPI_Finalize ();
  free (array);

  return EXIT_SUCCESS;
}
