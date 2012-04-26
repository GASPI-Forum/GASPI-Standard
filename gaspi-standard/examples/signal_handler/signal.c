#include <signal.h>
#include <stdlib.h>
#include <GASPI.h>

void
signalHandler (int sigint)
{
  gaspi_rank_t iProc = GASPI_NORANK;
  gaspi_rank_t nProc = GASPI_NORANK;

  gaspi_proc_rank (&iProc);
  gaspi_proc_num (&nProc);

  if (0 == iProc)
    {
      for (iProc = 1; iProc < nProc; ++iProc)
	{
	  gaspi_proc_kill (iProc, GASPI_BLOCK);
	}
    }

  gaspi_proc_term (GASPI_BLOCK);

  exit (EXIT_FAILURE);
}


int
main (int argc, char *argv[])
{
  gaspi_proc_init (GASPI_BLOCK);

  signal (SIGINT, &signalHandler);

  /* working phase */

  gaspi_proc_term (GASPI_BLOCK);

  return EXIT_SUCCESS;
}
