#ifndef UTIL_H
#define UTIL_H
void reduce(int *array
	    , int offset
	    , int size
	    , int M_SZ
	    , gaspi_rank_t iProc
	    );

void restrict_nBlocks(int *nBlocks
                      , int *mSize
                      , int sz
                      );

void get_offsets(int *istep
		 , int *itarget
		 , int *ioffset
		 , int *istage
		 , int *rstage
		 , int *nstage
		 , gaspi_rank_t iProc
		 , gaspi_rank_t nProc
		 );

#endif
