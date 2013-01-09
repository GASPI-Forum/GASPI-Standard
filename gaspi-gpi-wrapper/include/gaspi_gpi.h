#ifndef GASPI_GPI_H
#define GASPI_GPI_H
#include <gaspi.h>

typedef struct {
  int argc;
  char ** argv;
}argument_t;
/*#define GASPI_SUCCESS 0
#define GASPI_BLOCK 100
#define GASPI_TIMEOUT 1
#define GASPI_ERROR -1

typedef int gaspi_return_t;
typedef int gaspi_timeout_t;
typedef unsigned int gaspi_rank_t;

// weak symbols
extern gaspi_return_t gaspi_proc_init ( gaspi_configuration_t configuration, gaspi_timeout_t timeout);
extern gaspi_return_t gaspi_proc_term (gaspi_timeout_t timeout );
extern gaspi_return_t gaspi_proc_rank (gaspi_rank_t *rank);
extern gaspi_return_t gaspi_proc_num ( gaspi_rank_t *proc_num );*/

extern void gaspi_printf(char * format, ... );

// strong symbols
extern gaspi_return_t pgaspi_proc_init ( gaspi_configuration_t configuration, gaspi_timeout_t timeout);
extern gaspi_return_t pgaspi_proc_term (gaspi_timeout_t timeout );
#endif
