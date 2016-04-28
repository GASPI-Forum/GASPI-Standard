#include <omp.h>

#include "data.h"
#include "assert.h"
#include "math.h"



void data_compute(int mStart
		  , int mStop
		  , block_t *block
		  , int i
		  , double* work_array
		  , double* target_array
		  , int mSize
		  )
{
  int j, k;
  for (k = 0; k < mStop-mStart; k++) 	
    {
      const int jstart = block[i].pid * mSize;
      const int kstart = block[i].pid * mSize;  
#pragma simd
#pragma vector nontemporal
#pragma vector aligned      
      for (j = block[i].start-jstart; j < block[i].end-jstart; ++j)
	{
	  target_array_ELEM(j,kstart+k) = work_array_ELEM(k, j + jstart);
	}
    }

}


void data_validate(int mSize
		   , int mStart
		   , double* target_array
		   )
{
  int j, k;
  for (j = 0; j < M_SZ; j++)
    {
      for (k = 0;  k < mSize; k++) 	
	{
	  ASSERT(target_array_ELEM(k,j) == (double) (k + mStart) * M_SZ + j);
	}
    }
}

static void swap(double *a, double *b)
{
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

void sort_median(double *begin, double *end)
{
  double *ptr;
  double *split;
  if (end - begin <= 1)
    return;
  ptr = begin;
  split = begin + 1;
  while (++ptr != end) {
    if (*ptr < *begin) {
      swap(ptr, split);
      ++split;
    }
  }
  swap(begin, split - 1);
  sort_median(begin, split - 1);
  sort_median(split, end);
}

