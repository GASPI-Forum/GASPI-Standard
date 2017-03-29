#include <stdlib.h>
#include <GASPI.h>
#include <mpi.h>
#include "assert.h"
#include "success_or_die.h"
#include "topology.h"
#include "constant.h"
#include "now.h"
#include "testsome.h"
#include "util.h"
#include "queue.h"

static void init_array(int *array
		       , int size
		       , gaspi_rank_t iProc
		       , gaspi_rank_t nProc
		       )
{ 
  int j;
  for (j = 0; j < size; ++j)
    {
      array[j] = iProc;
    }
  for (j = 0; j < size; ++j)
    {
      array[size + j] = -1;
    }
}

static void validate(int *array
		     , int size
		     , gaspi_rank_t iProc
		     , gaspi_rank_t nProc
		     )
{
  /* validate */
  int i, j;
  int sum = 0;
  for (j = 0; j < nProc; ++j)
    {
      sum += j;
    }
  for (i = 0; i < size; ++i)
    {
      ASSERT(array[size + i] == sum);
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


  const gaspi_segment_id_t segment_id = 0;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id
					 , 2 * M_SZ * sizeof(int)
					 , GASPI_GROUP_ALL
					 , GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
		  );
  
  gaspi_pointer_t _ptr = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id, &_ptr));

  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));

  int *array = (int *) _ptr;
  init_array(array, M_SZ, iProc, nProc);
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  /* GASPI write - round-robin */
  double time = -now();

  gaspi_pointer_t bSend = (gaspi_pointer_t) array;  
  gaspi_pointer_t bRecv = (gaspi_pointer_t) (array + M_SZ);

  SUCCESS_OR_DIE (gaspi_allreduce (bSend
				   , bRecv
				   , M_SZ
				   , GASPI_OP_SUM
				   , GASPI_TYPE_INT
				   , GASPI_GROUP_ALL
				   , GASPI_BLOCK
				   ));

  time += now();
  printf("# RR  : iProc: %4d, size [byte]: %10d, time: %8.6f, total bandwidth [Mbyte/sec]: %8.0f\n"
	 , iProc, M_SZ, time, (double)(M_SZ*sizeof(int))/1024/1024/time); 
  
  validate(array, M_SZ, iProc, nProc);
      
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  MPI_Finalize();

  return EXIT_SUCCESS;
}

