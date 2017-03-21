#include <GASPI.h>
#include "assert.h"
#include "success_or_die.h"

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
 * set up targets for NWAY 
 * dissemination 
 */
void get_next(int nProc
	      , int NWAY
	      , int (*next)[NWAY]
	      )
{
  int j, k, i = 0;
  int rnd, width = 1;

  for (j = 0; j < nProc; j++)
    {
      for (k = 0; k < NWAY; k++)
	{
	  next[j][k] = -1;
	}
    }
  
  while (i < nProc)
    { 
      for (j = 0; j < width; j++)
        {
	  for (k = 0; k < NWAY; k++)
	    {
	      int nx = i+width+NWAY*j+k;
	      int ix = i+j;
	      if (nx < nProc && ix < nProc)
		{
		  next[ix][k] = nx;
		}
	    }
        }
      i += width;
      width *= NWAY;
    }
  
}

void wait_and_reset(gaspi_segment_id_t segment_id
		    , gaspi_notification_id_t nid
		    , gaspi_notification_t *val
			   )
{
  gaspi_notification_id_t id;
  SUCCESS_OR_DIE(gaspi_notify_waitsome (segment_id
					, nid
					, 1
					, &id
					, GASPI_BLOCK
					));
  ASSERT(nid == id);
  SUCCESS_OR_DIE(gaspi_notify_reset (segment_id
				     , id
				     , val
				     ));     
}	  


