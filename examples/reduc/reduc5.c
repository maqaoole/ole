//
double reduc5(double *restrict t, unsigned long long n)
{
  //32KB cache with 64 byte cache lines ==> 512 cache lines
  //sizeof(double) = 8 bytes ==> 8 doubles = 1 caheline

  register double sum0 = 0.0;
  register double sum1 = 0.0;
  register double sum2 = 0.0;
  register double sum3 = 0.0;
  register double sum4 = 0.0;
  
  unsigned long long i, j, b;
  unsigned long long nn = n * n;

  t = __builtin_assume_aligned(t, 32);

  //4KB Block = 64 cache lines
  for (b = 0; b < nn; b += 4096)
    for (i = 0; i < 4096; i += 8)
      {
	//1 cache line
	sum1 += t[b + i];
	sum2 += t[b + i + 1];
	sum3 += t[b + i + 2];
	sum4 += t[b + i + 3];
	
	sum1 += t[b + i + 4];
	sum2 += t[b + i + 5];
	sum3 += t[b + i + 6];
	sum4 += t[b + i + 7];
      }
  
  //Amdahl's law's a bitch!
  for (j = nn - (nn & 4095) + 1; j < nn; j++)
    sum0 += t[j];
  
  return sum0 + sum1 + sum2 + sum3 + sum4;
}
