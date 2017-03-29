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

void init_recv_state (int *recv_state
		      , gaspi_rank_t nProc
		      );

void init_block_data (int *nBlocks
		      , int *mSize
		      , gaspi_rank_t nProc
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

void write_stage (gaspi_segment_id_t segment_id
		  , int stage
		  , int *istep
		  , int *itarget
		  , gaspi_size_t *bSize
		  , gaspi_offset_t *offset
		  , gaspi_rank_t iProc
		  , gaspi_rank_t nProc
		  );

#endif
