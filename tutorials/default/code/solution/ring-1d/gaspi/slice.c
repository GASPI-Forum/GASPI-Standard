#include "gaspi/queue.h"
#include "gaspi/slice.h"
#include "gaspi/success_or_die.h"
#include "gaspi/testsome.h"

#include "assert.h"
#include "constant.h"
#include "data.h"
#include "topology.h"

void init_slices (slice *ssl, int NTHREADS)
{
  for (int l = 0; l < NTHREADS; ++l)
  {
    ssl[l].stage = 0;
    ssl[l].index = l+1;
    ssl[l].left = ssl + ((l + NTHREADS - 1) % NTHREADS);
    ssl[l].next = ssl + ((l + NTHREADS + 1) % NTHREADS);
    omp_init_lock (&ssl[l].lock);
  }

}


void handle_slice( slice *sl, double* array
		   , gaspi_notification_id_t* left_data_available
		   , gaspi_notification_id_t* right_data_available
		   , gaspi_segment_id_t segment_id
		   , gaspi_queue_id_t queue_id
		   , int NWAY, int NTHREADS, int num
		   )
{

  const int right_halo  = NTHREADS+1;
  const int left_halo   = 0;

  volatile int this_stage = get_ctr(&(sl->stage));
  volatile int next_stage = get_ctr(&(sl->next->stage));
  volatile int left_stage = get_ctr(&(sl->left->stage));

  ASSERT (this_stage < num);

  int const new_buffer_id = (this_stage + NWAY + 1 ) % NWAY;;
  int const old_buffer_id = (this_stage + NWAY) % NWAY;

  if (sl->index == left_halo + 1)
  {
    if (this_stage > next_stage)
    {
      return;
    }
    if (! test_or_die (segment_id, left_data_available[old_buffer_id], 1))
    {
      return;
    }
  }
  if (sl->index == right_halo - 1)
  {
    if (this_stage > left_stage)
    {
      return;
    }
    if (! test_or_die (segment_id, right_data_available[old_buffer_id], 1))
    {
      return;
    }
  }
  if ((sl->index > left_halo + 1) && (sl->index < right_halo - 1))
  {
    if (this_stage > left_stage || this_stage > next_stage)
    {
      return;
    }
  }

  data_compute (NTHREADS, array, new_buffer_id, old_buffer_id, sl->index);

  if (sl->index == left_halo + 1)
  {
    gaspi_rank_t iProc, nProc;
    SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
    SUCCESS_OR_DIE (gaspi_proc_num (&nProc));

    write_notify_and_cycle
      ( segment_id, array_OFFSET_left (new_buffer_id, left_halo + 1, 0), LEFT (iProc, nProc)
	, segment_id, array_OFFSET_left (new_buffer_id, right_halo, 0), VLEN * sizeof (double)
	, right_data_available[new_buffer_id], 1
      );
  }
  if (sl->index == right_halo - 1)
  {
    gaspi_rank_t iProc, nProc;
    SUCCESS_OR_DIE (gaspi_proc_rank (&iProc));
    SUCCESS_OR_DIE (gaspi_proc_num (&nProc));

    write_notify_and_cycle
      ( segment_id, array_OFFSET_right (new_buffer_id, right_halo - 1, 0), RIGHT (iProc, nProc)
	, segment_id, array_OFFSET_right (new_buffer_id, left_halo, 0), VLEN * sizeof (double)
	, left_data_available[new_buffer_id], 1
	);
  }

  increment_ctr(&(sl->stage));

}
