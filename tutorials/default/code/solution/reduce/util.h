#ifndef UTIL_H
#define UTIL_H

void reduce(int *array
	    , int size
	    , int sid
	    , int mSize
	    , int M_SZ
	    , gaspi_rank_t iProc
	    , int NWAY
	    , int (*next)[NWAY]
	    );
  
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


void wait_and_reset(gaspi_segment_id_t segment_id
		    , gaspi_notification_id_t nid
		    , gaspi_number_t num
		    , gaspi_notification_id_t *id
		    , gaspi_notification_t *val
		    );

#endif
