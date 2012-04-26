#include <stdio.h>
#include <stdlib.h>
#include <GASPI.h>

int
main (int argc, char *argv[])
{
  gaspi_proc_init ( GASPI_BLOCK );

  gaspi_rank_t iProc = GASPI_NORANK;
  gaspi_rank_t nProc = GASPI_NORANK;

  gaspi_proc_rank (&iProc);
  gaspi_proc_num (&nProc);

  printf ("Hello world from rank %i of %i!\n", iProc, nProc);

  gaspi_proc_term (GASPI_BLOCK);

  return EXIT_SUCCESS;
}
