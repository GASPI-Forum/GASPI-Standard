#include <omp.h>

#include "data_global.h"
#include "assert.h"
#include "math.h"

void init_global(block_t *block
		 , int *block_num
		 , int mSize
		 )
{
  int i;
  int num = 0;
  int pid = 0;
  int start = 0;
  int last  = 0;

  for (i = 1; i < M_SZ; ++i)
    {
      if (i % BL_SZ == 0 || i % mSize == 0)
	{
	  // prev block
	  omp_init_lock (&block[num].lock);
	  block[num].stage = -1;
	  block[num].end  = i;
	  block[num].start = start;
	  block[num].tid = -1;
	  block[num].pid = pid;


	  if (i % mSize == 0)
	    {	      
	      last = i;
	      pid++;
	    }

	  start = i;
	  num++;
	}
    }
  // add last block
  omp_init_lock (&block[num].lock);
  block[num].stage = -1;
  block[num].end  = i;
  block[num].start = start;
  block[num].tid = -1;
  block[num].pid = pid;
  num++;

  *block_num = num;

}



void data_init_global(int mStart
		      , int mStop
		      , block_t *block
		      , int block_num
		      , double* source_array
		      , double* work_array
		      , double* target_array
		      , int mSize
		      )
{  
  int i, j, k;

  for (i = 0; i < block_num; i++)
    {
      for (j = block[i].start; j < block[i].end; j++) 	
	{
	  for (k = 0;  k < mStop - mStart; k++) 	
	    {
	      source_array_ELEM(k,j) = (double) j * M_SZ + k + mStart;
	    }
	}

      for (j = block[i].start; j < block[i].end; j++) 	
	{
	  for (k = 0;  k < mStop - mStart; k++) 	
	    {
	      work_array_ELEM(k,j) = (double) -1 ;
	    }
	}

      for (j = block[i].start; j < block[i].end; j++) 	
	{
	  for (k = 0;  k < mStop - mStart; k++) 	
	    {
	      target_array_ELEM(k,j) = (double) -1 ;
	    }
	}

    }

}

