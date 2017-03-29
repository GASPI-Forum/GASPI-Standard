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


