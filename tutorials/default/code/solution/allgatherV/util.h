#ifndef UTIL_H
#define UTIL_H
void restrict_nBlocks(int *nBlocks
                      , int *mSize
                      , int sz
                      );

void init_recv_state (int *recv_state
		      , gaspi_rank_t nProc
		      );

void init_block_data (int *nBlocks
		      , int *mSize
		      , gaspi_rank_t nProc
		      );

#endif
