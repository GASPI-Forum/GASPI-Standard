#ifndef DATA_H
#define DATA_H

#include <omp.h>
#include "constant.h"

typedef struct 
{
  volatile int stage  __attribute__((aligned(64)));
  omp_lock_t lock __attribute__((aligned(64))); 
  int start, end, tid, pid;
} block_t;

#define POSITION(i,j) ((i) + mSize * (j))
#define array_OFFSET(i,j) (POSITION (i,j) * sizeof(double))
#define source_array_ELEM(i,j) ((double *)source_array)[POSITION (i,j)]
#define work_array_ELEM(i,j) ((double *)work_array)[POSITION (i,j)]
#define target_array_ELEM(i,j) ((double *)target_array)[POSITION (i,j)]


void data_compute(int mStart
		  , int mStop
		  , block_t *block
		  , int i
		  , double* work_array
		  , double* target_array
		  , int mSize
		  );


void data_validate(int mSize
		   , int mStart
		   , double* target_array
		   );


void sort_median(double *begin, double *end);


#endif
