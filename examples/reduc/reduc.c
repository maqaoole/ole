//Naive wrong memory pattern: rows before columns
double reduc(double *restrict t, unsigned long long n)
{
  double sum = 0.0;

  t = __builtin_assume_aligned(t, 32);
  
  for (unsigned long long col = 0; col < n; col++)
    for (unsigned long long row = 0; row < n; row++)
      {
	unsigned long long index = row * n + col;
	sum += t[index];
      }

  return sum;
}
