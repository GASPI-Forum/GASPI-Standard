#include <stdlib.h>
#include <GASPI.h>
#include <omp.h>
#include "success_or_die.h"
#include "assert.h"
#include "constant.h"
#include "now.h"
#include "waitsome.h"
#include "queue.h"

int
main (int argc, char *argv[])
{
  int i;
  
  SUCCESS_OR_DIE (gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t iProc;
  gaspi_rank_t nProc;

  SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
  SUCCESS_OR_DIE (gaspi_proc_num (&nProc));
  ASSERT (nProc == 2);

  omp_set_num_threads(nThreads);
  gaspi_rank_t target = 1 - iProc;  
  
  gaspi_number_t notification_max;
  SUCCESS_OR_DIE (gaspi_notification_num(&notification_max));
  
  const gaspi_segment_id_t segment_id_dst = 0;
  const gaspi_segment_id_t segment_id_ack = 1;

  /* dummy allocation, we use notification values as data */
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_dst
					 , 1
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  /* dummy allocation, we use notification values as data */
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_ack
					 , 1
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  
  double time = -now();

  /* initial notify */
#pragma omp parallel for
  for (i = 0; i < notification_max; ++i)
    {
      /* cycle queues  */
      notify_and_cycle (segment_id_dst
			, target
			, (gaspi_notification_id_t) i
			, 1
			);
    }
  
  int j, niter = 16;
  gaspi_segment_id_t segment_id = segment_id_dst;

  for (j = 0; j < niter; ++j)
    {      
#pragma omp parallel for
      for (i = 0; i < notification_max; ++i)
	{
	  wait_or_die(segment_id, i, 1);

	  if (j < niter - 1)
	    {
	      /* notify target for received notifications */
	      notify_and_cycle (1 - segment_id
				, target
				, (gaspi_notification_id_t) i
				, 1
				);
	    }
	}
      /* flip segment */
      segment_id = 1 - segment_id;
    }
  
  time += now();
  printf("# messages sent/recveived: %8d, total bi-directional message rate [#/sec]: %d\n"
	 , niter*notification_max, (int) ((double) 2*niter*notification_max/time)); 

  /* 
   * make sure that all our sends(notify) have been 
   * completed locally, before we exit main(). 
   */
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

