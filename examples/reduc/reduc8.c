#include <immintrin.h>

//
double reduc8(double *restrict t, unsigned long long n)
{
  double sum[4] = { 0.0, 0.0, 0.0, 0.0 };

  t = __builtin_assume_aligned(t, 32);
  
  //
  __asm__ volatile (
		    "xor    %%rdx, %%rdx;"
		    "xor    %%rcx, %%rcx;"
		    "xorpd  %%xmm0, %%xmm0;"
		    
		    "1:;"
		    
		    "movupd  0(%[ar], %%rdx), %%xmm1;"
		    "movupd 16(%[ar], %%rdx), %%xmm2;"
		    "movupd 32(%[ar], %%rdx), %%xmm3;"
		    "movupd 48(%[ar], %%rdx), %%xmm4;"
		    "movupd 64(%[ar], %%rdx), %%xmm5;"
		    "movupd 80(%[ar], %%rdx), %%xmm6;"
		    "movupd 96(%[ar], %%rdx), %%xmm7;"
		    "movupd 112(%[ar], %%rdx), %%xmm8;"
		    
		    "addpd  %%xmm1, %%xmm0;"
		    "addpd  %%xmm2, %%xmm0;"
		    "addpd  %%xmm3, %%xmm0;"
		    "addpd  %%xmm4, %%xmm0;"
		    "addpd  %%xmm5, %%xmm0;"
		    "addpd  %%xmm6, %%xmm0;"
		    "addpd  %%xmm7, %%xmm0;"
		    "addpd  %%xmm8, %%xmm0;"
		    
		    "add    $16, %%rcx;"
		    "add    $128, %%rdx;"
		    "cmp    %%rcx, %[ni];"
		    "jne 1b;"
		    
		    "vmovupd %%xmm0, 0(%[ou]);"
		    
		    : //Output
		      
		    : //Input
		      [ni] "r" (n * n),
		      [ar] "r" (t),
		      [ou] "r" (sum)
		      
		    : //Clobber
		      "cc", "memory", "rcx", "rdx", "xmm0", "xmm1"
		    );
  
  return sum[0] + sum[1];
}
