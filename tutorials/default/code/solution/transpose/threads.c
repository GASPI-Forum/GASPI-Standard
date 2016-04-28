/*
 * This file is part of a small series of tutorial,
 * which aims to demonstrate key features of the GASPI
 * standard by means of small but expandable examples.
 * Conceptually the tutorial follows a MPI course
 * developed by EPCC and HLRS.
 *
 * Contact point for the MPI tutorial:
 *                 rabenseifner@hlrs.de
 * Contact point for the GASPI tutorial:
 *                 daniel.gruenewald@itwm.fraunhofer.de
 *                 mirko.rahn@itwm.fraunhofer.de
 *                 christian.simmendinger@t-systems.com
 */

#include <stdlib.h>
#include <omp.h>
#include "assert.h"
#include "mm_pause.h"

/* fetch/add wrapper */
int my_add_and_fetch(volatile int *ptr, int val)
{
#ifdef GCC_EXTENSION
  int t = __sync_add_and_fetch(ptr, val);
  ASSERT(t >= 0);
  return t;
#else
  int t;
  //#pragma omp atomic capture 
#pragma omp critical (add_and_fetch)
  { 
    t = *ptr; 
    *ptr += val; 
  }
  t += val;
  ASSERT(t >= 0);
  return t;
#endif
}

int my_fetch_and_add(volatile int *ptr, int val)
{
#ifdef GCC_EXTENSION
  int t = __sync_fetch_and_add(ptr, val);
  ASSERT(t >= 0);
  return t;
#else
  int t;
  //#pragma omp atomic capture 
#pragma omp critical (fetch_and_add)
  { 
    t = *ptr; 
    *ptr += val; 
  }
  ASSERT(t >= 0);
  return t;
#endif
}

int this_is_the_first_thread(void)
{
  static volatile int shared_counter = 0;
  static int local_next     = 0;
#pragma omp threadprivate(local_next)

  const int nthreads = omp_get_num_threads(),
    first    = local_next;

  if(nthreads == 1)
    return 1;
  
  while(shared_counter < local_next)
    _mm_pause();

  local_next += nthreads;

  return(my_fetch_and_add(&shared_counter,1) == first);
}

int this_is_the_last_thread(void)
{
  static volatile int shared_counter = 0;
  static int local_next     = 0;
#pragma omp threadprivate(local_next)

  int const nthreads = omp_get_num_threads();

  if(nthreads == 1)
    return 1;

  while(shared_counter < local_next)
    _mm_pause();

  local_next += nthreads;

  return (my_add_and_fetch(&shared_counter,1) == local_next);
}
