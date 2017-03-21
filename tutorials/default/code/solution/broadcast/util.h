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

void get_next(int nProc
	      , int NWAY
	      , int (*next)[NWAY]
	      );

void wait_and_reset(gaspi_segment_id_t segment_id
		    , gaspi_notification_id_t nid
		    , gaspi_notification_t *val
		    );

#endif
