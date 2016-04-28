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

#include "success_or_die.h"

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
  gaspi_size_t const segment_size = VLEN * sizeof(double);

  /*
   TODO
   ====
   Create segment for GASPI_GROUP_ALL, obtain a pointer to the segment,
   cast the pointer to (double *) and initialize a 1-dim (double) array
   within the segment.
   */

  SUCCESS_OR_DIE(gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}
