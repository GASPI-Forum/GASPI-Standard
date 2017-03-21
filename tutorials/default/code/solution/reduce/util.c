#include <GASPI.h>
#include "assert.h"
#include "success_or_die.h"

/*
 * block-wise partial reduction
 */
void reduce(int *array
		   , int size
		   , int sid		   
		   , int mSize
		   , int M_SZ
		   , gaspi_rank_t iProc
		   , int NWAY
		   , int (*next)[NWAY]
		   )
{
  int i, j;
  for (j = 0; j < NWAY; ++j)
    {
      int source = next[iProc][j];
      if (source != -1)
	{
	  for (i = sid*mSize; i < sid*mSize+size; ++i)
	    {
	      array[i] += array[source*M_SZ+i];
	    }
	}
    }
}

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
      *NWAY = 7;
    }
  else if (M_SZ < mSize * N_SZ)
    {
      *NWAY = 3;
    }
  else
    {
      *NWAY = 1;
    }
}

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
      nRecv[j] = 0; 
      for (k = 0; k < NWAY; k++)
	{
	  if (next[j][k] != -1)
	    {
	      nRecv[j]++;
	    }
	}
    }    
}

void wait_and_reset(gaspi_segment_id_t segment_id
			   , gaspi_notification_id_t nid
			   , gaspi_number_t num
			   , gaspi_notification_id_t *id
			   , gaspi_notification_t *val
			   )
{
  SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
					, nid
					, num
					, id
					, GASPI_BLOCK
					));
  SUCCESS_OR_DIE(gaspi_notify_reset (segment_id
				     , *id
				     , val
				     ));     
  ASSERT(*val == 1);
}	  

