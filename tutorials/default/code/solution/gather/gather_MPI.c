#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "queue.h"

/* init */
static void init_array(int *array
		       , int size
		       , gaspi_rank_t nProc
		       , gaspi_rank_t iProc
		       )
{ 
  int j;
  for (j = 0; j < nProc*size; ++j)
    {
      array[j] = -1;
    }
  for (j = nProc*size; j < (nProc+1)*size; ++j)
    {
      array[j] = iProc;
    }
}

/* validate */
static void validate(int *array
		     , int size
		     , gaspi_rank_t nProc
		     , gaspi_rank_t iProc
		     )
{
  int i, j;
  if (iProc == 0)
    {
      for (i = 0; i < nProc; ++i)
	{
	  for (j = i*size; j < (i+1)*size; ++j)
	    {
	      ASSERT(array[j] == i);
	    }
	}
    }
}

int
main (int argc, char *argv[])
{  

  int nProc_MPI, iProc_MPI;
  gaspi_rank_t iProc, nProc;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &iProc_MPI);
  MPI_Comm_size (MPI_COMM_WORLD, &nProc_MPI);

  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));
  SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProc));

  ASSERT(iProc == iProc_MPI);
  ASSERT(nProc == nProc_MPI);

  int i, vlen = (nProc + 1) * M_SZ;
  gaspi_rank_t b_root = 0;
  
  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , vlen * sizeof(int)
					 , GASPI_GROUP_ALL
					 , GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
		  );
  
  gaspi_pointer_t _ptr = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id, &_ptr));

  int *array = (int *) _ptr;
  init_array(array, M_SZ, iProc, nProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  double time = -now();
  MPI_Gather(&array[nProc*M_SZ]
	     , M_SZ
	     , MPI_INT
	     , array
	     , M_SZ
	     , MPI_INT
	     , b_root
	     , MPI_COMM_WORLD
	     );
  
  time += now();
  printf("# REF : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(array, M_SZ, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  MPI_Finalize();

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

