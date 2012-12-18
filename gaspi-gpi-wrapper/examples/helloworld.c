#include <stdio.h>
#include <gaspi_gpi.h>

int main (int argc, char *argv[])
{
  gaspi_configuration_t config;
  config.argc = argc;
  config.argv = argv;

  gaspi_proc_init(config, GASPI_SUCCESS);
  gaspi_rank_t rank;
  gaspi_rank_t numOfRanks;
  gaspi_proc_rank(&rank);
  gaspi_proc_num(&numOfRanks);
  printf("Hello World I'am %d of %d\n",rank,numOfRanks);
  gaspi_proc_term(GASPI_SUCCESS);

  return 0;
}
