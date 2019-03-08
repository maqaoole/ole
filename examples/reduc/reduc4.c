//
double reduc4(double *restrict t, unsigned long long n)
{
  register double sum0 = 0.0;
  register double sum1 = 0.0;
  register double sum2 = 0.0;
  register double sum3 = 0.0;
  register double sum4 = 0.0;
  
  unsigned long long i, j;
  unsigned long long nn = n * n;

  t = __builtin_assume_aligned(t, 32);

  // 1/2 a caheline per iteration
  for (i = 0; i < nn; i += 4)
    {
      sum1 += t[i];
      sum2 += t[i + 1];
      sum3 += t[i + 2];
      sum4 += t[i + 3];
    }

  for (j = i + 1; j < nn; j++)
    sum0 += t[j];
  
  return sum0 + sum1 + sum2 + sum3 + sum4;
}
