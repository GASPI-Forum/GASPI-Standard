#include "GASPI.hpp"

GASPI_RETVAL GASPI_start( GASPI_Config_t & config
                        , int timeout)
{
  return GASPI_SUCCESS;
}
			
GASPI_RETVAL GASPI_shutdown(void)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_Substitute_Rank( GASPI_Config_t * conf
                                  , GASPI_Rank_t rank
                                  , char * hostname
                                  , int timeout )
{
  return GASPI_SUCCESS;
}

/* GASPI groups */

GASPI_RETVAL GASPI_emptyGroup( GASPI_Group_t& group )
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_addRankToGroup( GASPI_Group_t& group
                                 , GASPI_Rank_t rank )
{
  return GASPI_SUCCESS;
}
				 
GASPI_RETVAL GASPI_commitGroup( GASPI_Group_t& group
                              , int timeout )
{
  return GASPI_SUCCESS;
}
			      
GASPI_RETVAL GASPI_getMaxGroups(int& MaxGroups)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_getFreeGroups(int& FreeGroups);			      

/* GASPI segments */

GASPI_RETVAL GASPI_createSegment( int Seg
                                , int NUMASocket
                                , size_t Size
                                , int FD
                                , GASPI_SegCreat_t SegCreat
                                , GASPI_Group_t&   Group
                                , int timeout )
{
  return GASPI_SUCCESS;
}
				
GASPI_RETVAL GASPI_getDmaMemPtr( int Seg
                               , void* ptr )
{
  return GASPI_SUCCESS;
}                               
		      
/* GASPI single sided communictation*/

GASPI_RETVAL GASPI_Write( int locSeg
                        , unsigned long locOff
                        , int remRank
                        , int remSeg
                        , unsigned long remOff
                        , unsigned long size 
                        , int queue
                        , int timeout )
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_Write_Notify( int locSeg
                               , unsigned long locOff
                               , int remRank
                               , int remSeg
                               , unsigned long remOff
                               , unsigned long size
                               , int flag
                               , int flagRemSeg
                               , unsigned long flagRemOff
                               , int queue
                               , int timeout )
{
  return GASPI_SUCCESS;
}
			       
GASPI_RETVAL GASPI_Write_List( int n
                             , int *locSeg
                             , unsigned long *locOff
                             , int remRank
                             , int *remSeg
                             , unsigned long *remOff
                             , unsigned long *size 
                             , int queue
                             , int timeout )
{
  return GASPI_SUCCESS;
}
			     
GASPI_RETVAL GASPI_Write_List_Notify( int n
                                    , int *locSeg
                                    , unsigned long *locOff
                                    , int remRank
                                    , int *remSeg
                                    , unsigned long *remOff
                                    , unsigned long *size
                                    , int flag
                                    , int flagRemSeg
                                    , unsigned long flagRemOff
                                    , int queue
                                    , int timeout )
{
  return GASPI_SUCCESS;
}
				    
GASPI_RETVAL GASPI_Read( int locSeg
                       , unsigned long locOff
                       , int remRank
                       , int remSeg
                       , unsigned long remOff
                       , unsigned long size
                       , int queue
                       , int timeout )
{
  return GASPI_SUCCESS;
}
		       
GASPI_RETVAL GASPI_Read_List( int n
                            , int *locSeg
                            , unsigned long *locOff
                            , int remRank
                            , int *remSeg
                            , unsigned long *remOff
                            , unsigned long *size 
                            , int queue
                            , int timeout )
{
  return GASPI_SUCCESS;
}
			    
GASPI_RETVAL GASPI_Notify( int flag
                         , int remRank
                         , int flagRemSeg
                         , int flagRemRemOff
                         , int queue 
                         , int timeout )
{
  return GASPI_SUCCESS;
}
		       
GASPI_RETVAL GASPI_Wait( int queue
                       , int timeout )
{
  return GASPI_SUCCESS;
}
		       
GASPI_RETVAL GASPI_Wait_Flag_NE ( int val
                                , int locSeg
                                , int locOff
                                , int timeout )
{
  return GASPI_SUCCESS;
}
				
GASPI_RETVAL GASPI_openRequests( int queue
                               , int& openRequests)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_getNumQueues(int& NumQueues)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_getQueueDepth( int queue
                                , int& queueDepth )
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_getRank(int& Rank)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_getNumNodes(int& NumNodes)
{
  return GASPI_SUCCESS;
}

/* passive communication */

GASPI_RETVAL GASPI_sendPassive( int locSeg
                              , unsigned long locOff
                              , unsigned int remRank
                              , int size
                              , int tag )
{
  return GASPI_SUCCESS;
}
		     
GASPI_RETVAL GASPI_waitPassive( int timeout )
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_recvPassive( int locSeg
                              , unsigned long locOff
                              , int * remRank
                              , int * tag
                              , int timeout )
{
  return GASPI_SUCCESS;
}
		     
GASPI_RETVAL GASPI_MaxPassiveMsgSize(size_t& MaxMsgSize)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_OpenPassiveRequests(int& OpenRequests)
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_PassiveQueueDepth(int& QueueDepth)
{
  return GASPI_SUCCESS;
}

/* global atomics */

GASPI_RETVAL GASPI_resetCounter( unsigned long value
                               , int counter )
{
  return GASPI_SUCCESS;
}
		      
GASPI_RETVAL GASPI_fetchAddCounter( unsigned long *old_value
                                  , unsigned long add_value
                                  , int counter )
{
  return GASPI_SUCCESS;
}
			 
GASPI_RETVAL GASPI_cmpSwapCounter( unsigned long *old_value
                                 , unsigned long cmp_value
                                 , unsigned long new_value
                                 , int counter )
{
  return GASPI_SUCCESS;
}
			
int GASPI_getNumCounters(int& NumCounters)
{
  return GASPI_SUCCESS;
}

/* collectives */

GASPI_RETVAL GASPI_barrier( GASPI_Group_t& Group
                          , int timeout )
{
  return GASPI_SUCCESS;
}
		 
GASPI_RETVAL GASPI_allReduce( void *sendBuf
                            , void *recvBuf
                            , const unsigned char elemCnt
                            , GASPI_OP_t op
                            , GASPI_t Type
                            , GASPI_Group_t& Group
                            , int timeout )
{
  return GASPI_SUCCESS;
}

GASPI_RETVAL GASPI_allReduceUSER( void *sendBuf
                                , void *recvBuf
                                , const unsigned char elemCnt
                                , size_t elemSize
                                , void (*RedOp)( void * res
                                               , void * lop
                                               , void * rop )
                                , GASPI_Group_t& group
                                , int timeout )
{
  return GASPI_SUCCESS;
}
