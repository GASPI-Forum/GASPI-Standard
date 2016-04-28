
#ifndef CONSTANT_H
#define CONSTANT_H


#define MIN(x,y) ((x)<(y)?(x):(y))

/* alignment */
#ifdef USE_ALIGNMENT
static const int CL=8;
#else
static const int CL=1;
#endif

/* dimensions */
static const int nThreads  = 12;
static const int M_SZ      = 12*32*64;
static const int BL_SZ     = 32;
static const int NITER     = 250;
static const int NUM_READS = 4;

#endif
