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
		       , int *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int i, j;
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < bSize[i]; ++j)
	{
	  array[offset[i] + j] = -1;
	}
    }

  /* initialize local data */
  for (j = 0; j < bSize[iProc]; ++j)
    {
      array[offset[iProc] + j] = iProc;
    }
}

static void validate(int *array
		       , int *offset
		       , int *bSize
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		     )
{
  int i, j;

  /* validate */
  for (i = 0; i < nProc; ++i)
    {
      for (j = 0; j < bSize[i]; ++j)
	{	  
	  ASSERT(array[offset[i] + j] == i);
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

  int *offset = malloc(nProc * sizeof(int));
  ASSERT(offset != NULL);  

  int  *bSize = malloc(nProc * sizeof(int));
  ASSERT(bSize != NULL);
    
  int i, vlen = 0;

  srand(0);
  for (i = 0; i < nProc; ++i)
    {
      int rSize = rand() % M_SZ;
      offset[i]   = vlen;
      bSize[i]     = rSize;
      vlen       += bSize[i];
    }
    
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
  init_array(array, offset, bSize, iProc, nProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* MPI reference */
  double time = -now();
  MPI_Allgatherv(&array[offset[iProc]]
		 , bSize[iProc]
		 , MPI_INT
		 , array
		 , bSize
		 , offset
		 , MPI_INT
		 , MPI_COMM_WORLD);

  time += now();
  printf("# REF : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, bSize[iProc], time, (double)(vlen*sizeof(int))/1024/1024/time); 
  
  validate(array, offset, bSize, iProc, nProc);

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

