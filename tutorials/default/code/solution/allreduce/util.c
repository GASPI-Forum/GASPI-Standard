#include <GASPI.h>
#include "assert.h"
#include "success_or_die.h"

/*
 * partial reduction
 */
void reduce(int *array
	    , int offset
	    , int size
	    , int M_SZ
	    , gaspi_rank_t iProc
	    )
{
  int i;
  for (i = 0; i < size; ++i)
    {
      array[offset+i] += array[offset-M_SZ+i];
    }
}

void restrict_nBlocks(int *nBlocks
                      , int *mSize
                      , int sz
                      )
{
  if (sz <= *mSize * *nBlocks)
    {
      /* reduce number of blocks */
      if (sz > *mSize)
        {
          if (sz % *mSize == 0)
	    *nBlocks = sz / *mSize;
          else
	    *nBlocks = sz / *mSize + 1;
        }
      else
        {
          *nBlocks = 1;
	  *mSize = sz;
        }
    }
  else
    {
      /* increase message size */
      if (sz % *nBlocks == 0)
	*mSize = sz / *nBlocks;
      else
	*mSize = sz / *nBlocks + 1;
    }
}

void get_offsets(int *istep
		 , int *itarget
		 , int *ioffset
		 , int *istage
		 , int *rstage
		 , int *nstage
		 , gaspi_rank_t iProc
		 , gaspi_rank_t nProc
		 )
{
  int i, j;
  int l = 1;
  int m_sz = 1;
  int stage = 0;

  /* recursive doubling */
  while (m_sz + l <= nProc)
    {
      istage[stage] = stage;
      rstage[stage] = stage+1;
      istep[stage] = l;
      ioffset[stage] = m_sz;
      m_sz += l;
      l *= 2;
      stage++;
    }

  /* gather remaining blocks */
  int n_sz = ioffset[stage - 1] + istep[stage - 1];
  int k_sz = nProc - n_sz;
  for (l = stage - 1; l >= 0; --l)
    {
      if (k_sz / istep[l] >= 1)
	{
	  istage[stage] = l;
	  rstage[stage] = stage+1;
	  istep[stage] = istep[l];
	  ioffset[stage] = n_sz;
	  n_sz += istep[l];
	  k_sz = k_sz % istep[l];
	  stage++;
	}
    }

  /* set the actual comm target */
  for (l = 0; l < stage; ++l)
    {
      itarget[l] = (iProc - ioffset[l] + nProc) % nProc;
    }
  
  /* set total number of stages */
  *nstage = stage;

}


void write_stage (gaspi_segment_id_t segment_id
		  , int stage
		  , int *istep
		  , int *itarget
		  , gaspi_size_t *bSize
		  , gaspi_offset_t *offset
		  , gaspi_rank_t iProc
		  , gaspi_rank_t nProc
		  )
{
  gaspi_rank_t target = itarget[stage];
  gaspi_number_t num = istep[stage];
  gaspi_segment_id_t segment_id_local[num];
  gaspi_segment_id_t segment_id_remote[num];
  gaspi_offset_t offset_local[num];
  gaspi_offset_t offset_remote[num];
  gaspi_size_t size[num];
  int j;

  for (j = 0; j < num; ++j)    
    {
      int idx = (iProc + j) % nProc;
      size[j] = bSize[idx] * sizeof(int);
      offset_local[j] = offset[idx] * sizeof(int);
      offset_remote[j] = offset[idx] * sizeof(int);
      segment_id_local[j] = segment_id;
      segment_id_remote[j] = segment_id;
    }

  gaspi_notification_id_t notification = stage;
  gaspi_timeout_t const timeout = GASPI_BLOCK;
  gaspi_return_t ret;

  /* write, wait if required and re-submit */
  while ((ret = ( gaspi_write_list_notify( num
					   , segment_id_local
					   , offset_local
					   , target
					   , segment_id_remote
					   , offset_remote
					   , size
					   , segment_id
					   , notification
					   , 1
					   , 0
					   , timeout
					   )
		  )) == GASPI_QUEUE_FULL)
    {
      SUCCESS_OR_DIE (gaspi_wait (0,
				  GASPI_BLOCK));
    }
  ASSERT (ret == GASPI_SUCCESS);
}

