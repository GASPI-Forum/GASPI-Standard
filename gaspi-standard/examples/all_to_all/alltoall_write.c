#include <stdlib.h>
#include <GASPI.h>
#include <success_or_die.h>
#include <wait_if_queue_full.h>

extern void dump (int *arr, int nProc);

int
main (int argc, char *argv[])
{
  ASSERT (gaspi_proc_init (GASPI_BLOCK));

  gaspi_rank_t iProc;
  gaspi_rank_t nProc;

  ASSERT (gaspi_proc_rank (&iProc));
  ASSERT (gaspi_proc_num (&nProc));

  gaspi_notification_id_t notification_max;
  ASSERT (gaspi_notification_num(&notification_max));

  if (notification_max < (gaspi_notification_id_t)nProc)
    {
      exit (EXIT_FAILURE);
    }

  ASSERT (gaspi_group_commit (GASPI_GROUP_ALL, GASPI_BLOCK));

  const gaspi_segment_id_t segment_id_src = 0;
  const gaspi_segment_id_t segment_id_dst = 1;

  const gaspi_size_t segment_size = nProc * sizeof(int);

  ASSERT (gaspi_segment_create ( segment_id_src, segment_size
                               , GASPI_GROUP_ALL, GASPI_BLOCK
                               , GASPI_ALLOC_DEFAULT
                               )
         );
  ASSERT (gaspi_segment_create ( segment_id_dst, segment_size
                               , GASPI_GROUP_ALL, GASPI_BLOCK
                               , GASPI_ALLOC_DEFAULT                               
                               )
         );

  int *src = NULL;
  int *dst = NULL;

  ASSERT (gaspi_segment_ptr (segment_id_src, &src));
  ASSERT (gaspi_segment_ptr (segment_id_dst, &dst));

  const gaspi_queue_id_t queue_id = 0;

  for (gaspi_rank_t rank = 0; rank < nProc; ++rank)
    {
      src[rank] = iProc * nProc + rank;

      const gaspi_offset_t offset_src = rank * sizeof (int);
      const gaspi_offset_t offset_dst = iProc * sizeof (int);
      const gaspi_notification_id_t notify_ID = rank;

      wait_if_queue_full (queue_id, 2);

      const gaspi_notification_t notify_val = 1;

      ASSERT
        (gaspi_write_notify ( segment_id_src, offset_src
                            , rank, segment_id_dst, offset_dst
                            , sizeof (int), notify_ID, notify_val
                            , queue_id, GASPI_BLOCK
                            )
        );
    }

  gaspi_notification_id_t notify_cnt = nProc;
  gaspi_notification_id_t first_notify_id;

  while (notify_cnt > 0)
  {
    ASSERT (gaspi_notify_waitsome (segment_id_dst, 0, nProc
				   , GASPI_BLOCK, &first_notify_id));

    gaspi_notification_id_t notify_val = 0;

    ASSERT (gaspi_notify_reset (segment_id_dst, first_notify_id
                               , &notify_val));

    if (notify_val != 0)
       {
         --notify_cnt;
       }
  }

  dump (dst, nProc);

  ASSERT (gaspi_wait (queue_id, GASPI_BLOCK));

  ASSERT (gaspi_barrier (GASPI_GROUP_ALL, GASPI_BLOCK));

  ASSERT (gaspi_proc_term (GASPI_BLOCK));

  return EXIT_SUCCESS;
}
