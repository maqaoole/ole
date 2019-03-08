//
double reduc2(double *restrict t, unsigned long long n)
{
  double sum = 0.0;

  t = __builtin_assume_aligned(t, 32);

  //Let he compiler do the computation (LEA?)
  double (*p_t)[n] = (double (*)[n]) t;
  
  for (unsigned long long row = 0; row < n; row++)
    for (unsigned long long col = 0; col < n; col++)
      {
	unsigned long long index = row * n + col;
	sum += t[index];
      }

  return sum;
}
