#include <stdlib.h>
#include <GASPI.h>
#include <omp.h>
#include "success_or_die.h"
#include "assert.h"
#include "constant.h"
#include "now.h"
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

  /* dummy allocation, we use notification values as data */
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_dst
					 , 1
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  
  double time = -now();
  
  /* notify target for notification_max integers */
#pragma omp parallel
  {
#pragma omp for
    for (i = 0; i < notification_max; ++i)
      {
	/* cycle queues  */
	notify_and_cycle (segment_id_dst
			  , target
			  , (gaspi_notification_id_t) i
			  , 1
			  );
      }
  
    /*
     * TODO: wait for all notifications. 
     */
    
  }
    
  time += now();
  printf("# messages sent/recveived: %8d, total bi-directional message rate [#/sec]: %d\n"
	 , notification_max, (int) ((double) 2*notification_max/time)); 

  for (i = 0; i < notification_max; ++i)
    {
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id_dst
					 , (gaspi_notification_id_t) i
					 , &value
					 ));
      ASSERT(value == 1);
    }

  /* TODO: Explain: Why do we need to wait for the queues ? */
  wait_for_flush_queues();

  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

