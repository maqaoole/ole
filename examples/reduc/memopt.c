/*
  This code presents examples of how to and how not to access memory.
  Enjoy!
  
  REDUC:     sum += t[i][j]; or sum += t[i];
    
    reduc
    reduc1
    reduc2
    reduc3
    reduc4
    reduc5
   
  GPL 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <emmintrin.h>

#include "rdtsc.h"

#include "protos.h"

#define META 31

// BEGIN MEASURE MACRO
#define measure(title, fct, a, nb, s, sam, cycle)	\
  					 \
  for (int i = 0; i < META; i++)	 \
    {					 \
      before = rdtsc();			 \
      					 \
      fct(a, nb);			 \
      					 \
      after = rdtsc();			 \
      					 \
      sam[i] = (after - before);	 \
    }					 \
  					 \
  sort(sam, META);			 \
  					 \
  printf("%s:\n", title);						\
  									\
  printf("\t%llu MBytes\tmin: %.2lf Cycles\tmax: %.2lf Cycles\tavg: %.2lf Cycles\tmed: %.2lf Cycles\n", s >> 20, \
	 sam[0],							\
	 sam[META - 1],							\
	 (sam[0] + sam[META - 1]) / 2,					\
	 sam[(META >> 1) + 1]);						\
  									\
  printf("\tBytes/Cycle: %.2lf\tCycles/Byte: %.2lf\n", s / sam[(META >> 1) + 1], sam[(META >> 1) + 1] / s); \
  printf("\tCycles/Iter: %.2lf\tIters/Cycle: %.2lf\n\n", sam[(META >> 1) + 1] / nb * nb, nb * nb / sam[(META >> 1) + 1]); \
  cycle = sam[(META >> 1) + 1];						\

// END MEASURE MACRO

//Dumb sort
void sort(double *restrict t, int n)
{
  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
      if (t[j] < t[i])
	{
	  double tmp = t[i];

	  t[i] = t[j];
	  t[j] = tmp;
	}
}

//Inline loop if possible!
static inline void init_array(double *restrict t, unsigned long long n)
{
  for (unsigned long long i = 0; i < n * n; i++)
    t[i] = 1.0;
}

//
int main(int argc, char **argv)
{
  if (argc < 2)
    return printf("OUPS: %s [n: nb elements]\n", argv[0]), -1;

  double ret = 0.0;
  double after, before;				
  double samples[META];
  double times[META * 2];
  unsigned long long n = atoll(argv[1]);
  double *restrict t = _mm_malloc(sizeof(double) * n * n, 32); //Aligned memory
  unsigned long long size = (sizeof(double) * n * n);
  
  if (t)
    {
      printf("Array size: %llu KBytes, %llu MBytes\n\n", size >> 10, size >> 20);

      printf("Initializing array ...\n");
      
      init_array(t, n);

      printf("Measuring ...\n");
      
      measure("REDUC" , reduc , t, n, size, samples, times[0]);
      measure("REDUC1", reduc1, t, n, size, samples, times[1]);
      measure("REDUC2", reduc2, t, n, size, samples, times[2]);
      measure("REDUC3", reduc3, t, n, size, samples, times[3]);
      measure("REDUC4", reduc4, t, n, size, samples, times[4]);
      measure("REDUC5", reduc5, t, n, size, samples, times[5]);
      measure("REDUC6", reduc6, t, n, size, samples, times[6]);
      measure("REDUC7", reduc7, t, n, size, samples, times[7]);
      measure("REDUC8", reduc8, t, n, size, samples, times[8]);
      measure("REDUC9", reduc9, t, n, size, samples, times[9]);
      measure("REDUC10", reduc10, t, n, size, samples, times[10]);
      measure("REDUC11", reduc11, t, n, size, samples, times[11]);
      
      sort(times, 12);

      double min = times[0];
      
      printf("Speedups compared to the best version (best to worst):\n");
      for (int i = 1; i < 12; i++)
	printf("\t%lf\n", times[i] / min);
      
      _mm_free(t);
    }
  
  return 0;
}

