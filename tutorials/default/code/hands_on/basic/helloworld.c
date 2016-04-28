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
  SUCCESS_OR_DIE(gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t rank;
  gaspi_rank_t num;
  /*
   TODO
   ====
   say hello from rank x of y
   */

  SUCCESS_OR_DIE(gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}
