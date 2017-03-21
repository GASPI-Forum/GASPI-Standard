#include "assert.h"

/*
 * restrict NWAY dissemination, when dropping 
 * below minimal message size
 */
void restrict_NWAY(int *NWAY
		   , int mSize
		   , int M_SZ
		   , int N_SZ
		   )
{
  if (M_SZ < mSize)
    {
      *NWAY = 9;
    }
  else if (M_SZ < mSize * N_SZ)
    {
      *NWAY = 5;
    }
  else
    {
      *NWAY = 2;
    }
}

/* 
 * restrict number of blocks (nBlocks), when dropping 
 * below minimal message size (mSize)
 */
void restrict_nBlocks(int *nBlocks
		      , int *mSize
		      , int M_SZ
		      , int *lSize
		      )
{
  if (M_SZ < *mSize * *nBlocks)
    {
      /* reduce number of blocks */
      if (M_SZ > *mSize)
	{
	  *nBlocks = M_SZ / *mSize;
	  *lSize = M_SZ % *mSize;
	  if (*lSize != 0)
	    {
	      (*nBlocks)++;
	    }
	  else
	    {
	      *lSize = *mSize;
	    } 
	}
      else
	{
	  *nBlocks = 1;
	  *lSize = M_SZ;
	}
    }
  else
    {
      /* increase message size */
      *mSize = M_SZ / *nBlocks;
      *lSize = M_SZ % *mSize;
      if (*lSize != 0)
	{
	  (*nBlocks)++;
	}
      else
	{
	  *lSize = *mSize;
	} 
    }
  ASSERT((*nBlocks - 1) * *mSize + *lSize == M_SZ);
}

static int get_n_next(int idx
		      , int NWAY
		      , int (*next)[NWAY]
		      )
{
  int k, i = 1;
  for (k = 0; k < NWAY; k++)
    {
      int j = next[idx][k];
      if (j != -1)
	{
	  i +=  get_n_next(j, NWAY, next); 
	}
    }
  return i;
}
/* 
 * set up source and target for NWAY 
 * dissemination 
 */
void get_p_next(int nProc
		, int NWAY
		, int *prev
		, int (*next)[NWAY]
		, int *nRecv
		)
{
  int j, k, i = 0;
  int rnd, width = 1;

  for (j = 0; j < nProc; j++)
    {
      prev[j] = -1;
      for (k = 0; k < NWAY; k++)
	{
	  next[j][k] = -1;
	}
    }

  /* set prev, next */
  while (i < nProc)
    { 
      for (j = 0; j < width; j++)
        {
	  for (k = 0; k < NWAY; k++)
	    {
	      int nx = i+width+NWAY*j+k;
	      int ix = i+j;
	      if (nx < nProc && ix < nProc )
		{
		  ASSERT(prev[nx] == -1);
		  next[ix][k] = nx;
		  prev[nx] = ix;
		}
	    }
        }
      i += width;
      width *= NWAY;
    }

  /* number of incoming messages */
  for (j = 0; j < nProc; j++)
    {
      nRecv[j] =  get_n_next(j, NWAY, next) - 1; 
    }
    
}

