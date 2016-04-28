#ifndef DATA_GLOBAL_H
#define DATA_GLOBAL_H

#include <omp.h>
#include "constant.h"
#include "data.h"

void init_global(block_t *block
		      , int *block_num
		      , int mSize
		      );


void data_init_global(int mStart
		      , int mStop
		      , block_t *block
		      , int block_num
		      , double* source_array
		      , double* work_array
		      , double* target_array
		      ,int mSize
		      );

#endif
