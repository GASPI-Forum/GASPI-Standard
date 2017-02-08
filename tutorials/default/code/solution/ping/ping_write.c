#include <stdlib.h>
#include <GASPI.h>
#include <success_or_die.h>
#include "constant.h"

static double mysecond()
{
  struct timeval tp;
  int i;
  i = gettimeofday(&tp,NULL);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

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

  const gaspi_queue_id_t queue_id = 0;  gaspi_number_t queue_num;
  SUCCESS_OR_DIE(gaspi_queue_num (&queue_num));
  
  gaspi_notification_id_t notification_max;
  SUCCESS_OR_DIE (gaspi_notification_num(&notification_max));

  const gaspi_segment_id_t segment_id_src = 0;
  const gaspi_segment_id_t segment_id_dst = 1;
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_src
					 , notification_max * sizeof(double)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  SUCCESS_OR_DIE (gaspi_segment_create ( segment_id_dst
					 , notification_max * sizeof(double)
					 , GASPI_GROUP_ALL, GASPI_BLOCK
					 , GASPI_ALLOC_DEFAULT
					 )
	  );
  
  double *src = NULL;
  double *dst = NULL;
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_src, &src));
  SUCCESS_OR_DIE (gaspi_segment_ptr (segment_id_dst, &dst));


#pragma omp parallel
  for (i = 0; i < notification_max; ++i)
    {
      src[i] = 1;
      dst[i] = 0;
    }

  gaspi_rank_t target = 1 - iProc;  
  double t1 = -mysecond();

  omp_set_num_threads(nThreads);
  
  /* write notification_max doubles */
#pragma omp parallel
  for (i = 0; i < notification_max; ++i)
    {
      int tid = omp_get_thread_num();
      gaspi_queue_id_t queue_id = tid % queue_num;
      gaspi_offset_t  offset_local  = i * sizeof(double);
      gaspi_offset_t  offset_remote = i * sizeof(double);
      
      write_and_wait ( segment_id_src
		       , offset_local
		       , target
		       , segment_id_dst
		       , offset_remote
		       , sizeof(double)
		       , queue_id
		       );
    }

  /* notify all triggered queues */
  for (i = 0; i < MIN(nThreads, queue_num); ++i)
    {
      gaspi_notify (segment_id_dst
		    , target
		    , (notification_id_t) i
		    , 1
		    , (gaspi_queue_id_t) i
		    , timeout
		    );
    }

  /* wait for notify */
  num_received = 0;
  while (num_received < MIN(nThreads, queue_num)
    {
      gaspi_notification_id_t id;
      SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id_dst,
					    0,
					    notification_max,
					    &id,
					    GASPI_BLOCK));
      gaspi_notification_t value;
      SUCCESS_OR_DIE(gaspi_notify_reset (segment_id_dst,
					 id,
					 &value));
      ASSERT(value == 1);

      num_received++;
    }
  
  t1 += mysecond();
  printf("## messages: %d time: %10.6f messages/sec: %d\n"
	 , notification_max, t1, (int) ((double) notification_max/t1)); 
    

  /* validate */
  for (i = 0; i < notification_max; ++i)
    {
      ASSERT(dst[i] == 1);
    }

  wait_for_flush_queues();
  
  SUCCESS_OR_DIE (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  SUCCESS_OR_DIE (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}

