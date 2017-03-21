#ifndef UTIL_H
#define UTIL_H

void restrict_NWAY(int *NWAY
		   , int mSize
		   , int M_SZ
		   , int N_SZ
		   );


void restrict_nBlocks(int *nBlocks
		      , int *mSize
		      , int M_SZ
		      , int *lSize
		      );

void get_p_next(int nProc
		, int NWAY
		, int *prev
		, int (*next)[NWAY]
		, int *nRecv
		);

#endif
