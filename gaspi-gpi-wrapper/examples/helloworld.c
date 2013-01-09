#include <stdio.h>
#include <gaspi_gpi.h>

int main (int argc, char *argv[])
{
  gaspi_configuration_t config;
  argument_t arg;
  
  arg.argc = argc;
  arg.argv = argv;
  config.user_defined = &arg;
  gaspi_proc_init(config, GASPI_BLOCK);
  gaspi_rank_t rank;
  gaspi_rank_t numOfRanks;
  gaspi_proc_rank(&rank);
  gaspi_proc_num(&numOfRanks);
  gaspi_printf("Hello World I'am %d of %d\n",rank,numOfRanks);
  gaspi_proc_term(GASPI_BLOCK);

  return 0;
}
