#include <immintrin.h>

//
double reduc11(double *restrict t, unsigned long long n)
{
  double sum[4] = { 0.0, 0.0, 0.0, 0.0 };

  t = __builtin_assume_aligned(t, 32);
  
  //
  __asm__ volatile (
		    "xor    %%rdx, %%rdx;"
		    "xor    %%rcx, %%rcx;"
		    "vxorpd  %%ymm0, %%ymm0, %%ymm0;"

		    //Unroll of 8 (4 elements per load)
		    "1:;" //4 cachelines (sizeof(ymm) = 256b = 32B = 1/2 cache line)
		    
		    "vaddpd  0(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 32(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 64(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 96(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 128(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 160(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 192(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    "vaddpd 224(%[ar], %%rdx), %%ymm0, %%ymm0;"
		    		    
		    "add    $256, %%rdx;"
		    "add    $32, %%rcx;"
		    "cmp    %%rcx, %[ni];"
		    "jne 1b;"
		    
		    "vmovupd %%ymm0, 0(%[ou]);"
		    
		    : //Output
		      
		    : //Input
		      [ni] "r" (n * n),
		      [ar] "r" (t),
		      [ou] "r" (sum)
		      
		    : //Clobber
		      "cc", "memory", "rcx", "rdx", "ymm0", "ymm1"
		    );
  
  return sum[0] + sum[1] + sum[2] + sum[3];
}
