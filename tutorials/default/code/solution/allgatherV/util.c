#include <GASPI.h>
#include "assert.h"

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
            {
	      *nBlocks = sz / *mSize;
            }
          else
            {
	      *nBlocks = sz / *mSize + 1;
            }
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
	{
	  *mSize = sz / *nBlocks;
	}
      else
	{
	  *mSize = sz / *nBlocks + 1;
	}
    }
}


void init_recv_state (int *recv_state
		      , gaspi_rank_t nProc
		      )
{ 
  int i;
  /* initialize receive status for all data chunks */ 
  for (i = 0; i < nProc; ++i)
    {
      recv_state[i] = 0;
    }
}


void init_block_data (int *nBlocks
		      , int *mSize
		      , gaspi_rank_t nProc
		      )
{ 
  int i;
  /* initialize block status  */ 
  for (i = 0; i < nProc; ++i)
    {
      nBlocks[i] = 512;
      mSize[i] = 4096;
    }
}

