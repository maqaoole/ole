//
double reduc10(double *restrict t, unsigned long long n)
{
  double sum = 0.0;

  t = __builtin_assume_aligned(t, 32);
			       
#pragma omp simd
  for (unsigned long long i = 0; i < n * n; i++)
    sum += t[i];

  return sum;
}
