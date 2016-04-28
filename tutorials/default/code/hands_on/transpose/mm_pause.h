#ifndef MM_PAUSE_H
#define MM_PAUSE_H

#include <stdlib.h>

static __inline void _mm_pause (void)
{
  __asm__ __volatile__ ("rep; nop" : : );
}


#endif
