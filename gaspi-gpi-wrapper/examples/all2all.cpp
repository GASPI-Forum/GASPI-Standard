
#include "gaspi.h"
#include "GpiLogger.h"
#include <stdio.h>

#define GPI_SEGMENT 0

void dump (const int *arr, const int iProc, const int nProc, const char *msg)
{
  gpi_printf ("%s %i:", msg, iProc);
  for (int i = 0; i < nProc; ++i)
    gpi_printf (" %2i", arr[i]);
  gpi_printf ("\n");
}

int main (int argc, char *argv[])
{
  gaspi_configuration_t  config = { 0 };
  //argc, argv, "", (1UL << 30)

  gaspi_proc_init (config, GASPI_BLOCK); // 1 GiB DMA enabled memory per node

  gaspi_rank_t iProc, nProc;
  gaspi_proc_rank (&iProc);
  gaspi_proc_num (&nProc);

  void* temp_ptr;
  gaspi_segment_ptr(GPI_SEGMENT, &temp_ptr);

  int *mem = (int *) temp_ptr;  // begin of DMA enabled memory
  int *src = mem;               // offset 0
  int *dst = mem + nProc;       // offset nProc * sizeof(int)

  for (gaspi_rank_t p = 0; p < nProc; ++p)
    {
      src[p] = iProc * nProc + p;

      const unsigned long locOff = p * sizeof (int);
      const unsigned long remOff = (nProc + iProc) * sizeof (int);

      gaspi_write(GPI_SEGMENT, locOff, p, GPI_SEGMENT, remOff, sizeof (int), 0, GASPI_BLOCK);
    }

  gaspi_wait (0, GASPI_BLOCK);
  gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK);

  dump (src, iProc, nProc, "src");
  dump (dst, iProc, nProc, "dst");

  gaspi_proc_term (GASPI_BLOCK);

  return EXIT_SUCCESS;
}

/* > gcc --std=c99 alltoall.c -I $GPI_HOME/include -L $GPI_HOME/lib64 -lGPI -libverbs15
 * > cp a.out $GPI_HOME/bin
 * > getnode -n 4
 * > $GPI_HOME/bin/a.out
 * [...collected and sorted output...]
 * src 0:  0  1  2  3   dst 0:  0  4  8 12
 * src 1:  4  5  6  7   dst 1:  1  5  9 13
 * src 2:  8  9 10 11   dst 2:  2  6 10 14
 * src 3: 12 13 14 15   dst 3:  3  7 11 15
 */
