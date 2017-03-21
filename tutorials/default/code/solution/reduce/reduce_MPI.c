#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "queue.h"

static void init_array(int *array
		       , int size
		       , gaspi_rank_t iProc
		       )
{ 
  int j;
  for (j = 0; j < size; ++j)
    {
      array[j] = iProc;
    }

  for (j = size; j < 2*size; ++j)
    {
      array[j] = -1;
    }
}

static void validate(int *array
		     , int size
		     , gaspi_rank_t nProc
		     , gaspi_rank_t iProc
		     )
{
  int i, j;
  int sum = 0;
  for (j = 0; j < nProc; ++j)
    {
      sum += j;
    }
  
  /* validate */
  if (iProc == 0)
    {
      for (j = size; j < 2*size; ++j)
	{
	  ASSERT(array[j] == sum);
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

  int i, vlen = 2 * M_SZ;
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
  init_array(array, M_SZ, iProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* MPI reference */
  double time = -now();
  MPI_Reduce(&array[0]
	     , &array[M_SZ]
	     , M_SZ
	     , MPI_INT
	     , MPI_SUM
	     , b_root
	     , MPI_COMM_WORLD
	     );
  
  time += now();
  printf("# REF : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(&array[M_SZ], M_SZ, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  MPI_Finalize();

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

