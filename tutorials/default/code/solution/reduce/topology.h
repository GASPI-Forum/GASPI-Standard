#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#define NGB(iProc,nProc,iDir) ((iProc + nProc + (iDir)) % nProc)
#define RIGHT(iProc,nProc) ((iProc + nProc + 1) % nProc)
#define LEFT(iProc,nProc)  ((iProc + nProc - 1) % nProc)

#endif
