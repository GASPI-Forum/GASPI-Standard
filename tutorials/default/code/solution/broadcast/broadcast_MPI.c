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
      array[j] = -1;
    }

  /* initialize local data */
  if (iProc == 0)
    {
      for (j = 0; j < size; ++j)
	{
	  array[j] = 0;
	}
    }
}

static void validate(int *array
		     , int size
		     )
{
  int i, j;

  /* validate */
  for (j = 0; j < size; ++j)
    {	  
      ASSERT(array[j] == 0);
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

  int i, vlen = M_SZ;
    
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
  init_array(array, vlen, iProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* MPI reference */
  double time = -now();
  MPI_Bcast(array
	    , vlen
	    , MPI_INT
	    , 0
	    , MPI_COMM_WORLD);
  
  time += now();
  printf("# REF : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, vlen, time, (double)(vlen*sizeof(int))/1024/1024/time); 
  
  validate(array, vlen);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

