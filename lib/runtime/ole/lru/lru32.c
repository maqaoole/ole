#include "lru32.h"

//Done manually (Why ? It's faster !)
unsigned col_masks1024[32] = { 0x7FFFFFFF, 0xBFFFFFFF, 0xDFFFFFFF, 0xEFFFFFFF, 
			       0xF7FFFFFF, 0xFBFFFFFF, 0xFDFFFFFF, 0xFEFFFFFF,
			       0xFF7FFFFF, 0xFFBFFFFF, 0xFFDFFFFF, 0xFFEFFFFF,
			       0xFFF7FFFF, 0xFFFBFFFF, 0xFFFDFFFF, 0xFFFEFFFF,
			       0xFFFF7FFF, 0xFFFFBFFF, 0xFFFFDFFF, 0xFFFFEFFF, 
			       0xFFFFF7FF, 0xFFFFFBFF, 0xFFFFFDFF, 0xFFFFFEFF,
			       0xFFFFFF7F, 0xFFFFFFBF, 0xFFFFFFDF, 0xFFFFFFEF,
			       0xFFFFFFF7, 0xFFFFFFFB, 0xFFFFFFFD, 0xFFFFFFFE };

//
void init_zero1024(refmatx1024 *m)
{
  for (int i = 0; i < 32; i++)
    m->matx[i] = 0x00000000;
}

//
int get_zero_pos1024(refmatx1024 *m)
{
  for (int i = 0; i < 32; i++)
    if (!m->matx[i])
      return i;
}

//
int update_LRU_line1024(refmatx1024 *m, u_int line)
{
  m->matx[line] = 0xFFFFFFFF;
  
  for (int i = 0; i < 32; i++)
    m->matx[i] &= col_masks1024[line];
  
  return 0;
}

//
void print_LRU_matx1024(refmatx1024 *m)
{
  for (int i = 0; i < 32; i++)
    printf("0x%08x\n", m->matx[i]);
}
