#ifndef GASPI_GPI_H
#define GASPI_GPI_H
#include <gaspi.h>

typedef struct {
  int argc;
  char ** argv;
}argument_t;

extern void gaspi_printf(char * format, ... );

// strong symbols
extern gaspi_return_t pgaspi_proc_init ( gaspi_configuration_t configuration, gaspi_timeout_t timeout);
extern gaspi_return_t pgaspi_proc_term (gaspi_timeout_t timeout );
extern gaspi_return_t pgaspi_barrier(gaspi_group_t group, gaspi_timeout_t timeout);
#endif
