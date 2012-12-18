#include <stdio.h>
#include <gaspi_gpi.h>

int main (int argc, char *argv[])
{
  gaspi_configuration_t config;
  config.argc = argc;
  config.argv = argv;

  gaspi_proc_init(config, GASPI_SUCCESS);
  printf("Hello World\n");
  gaspi_proc_term(GASPI_SUCCESS);

  return 0;
}
