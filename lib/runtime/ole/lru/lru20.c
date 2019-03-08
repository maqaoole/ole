#include "lru20.h"

//Done manually (Why ? It's faster !)
unsigned col_masks20x20[20] = { 0x000FFFFE, 0x000FFFFD, 0x000FFFFB, 0x000FFFF7, 
				0x000FFFEF, 0x000FFFDF, 0x000FFFBF, 0x000FFF7F,
				0x000FFEFF, 0x000FFDFF, 0x000FFBFF, 0x000FF7FF,
				0x000FEFFF, 0x000FDFFF, 0x000FBFFF, 0x000F7FFF,
				0x000EFFFF, 0x000DFFFF, 0x000BFFFF, 0x0007FFFF };

//
int get_zero_pos20x20(refmatx20x20 *m)
{
  for (int i = 0; i < 20; i++)
    if (!m->matx[i])
      return i;
}

//
int update_LRU_line20x20(refmatx20x20 *m, u_int line)
{
  m->matx[line] = 0x000FFFFF;
  
  for (int i = 0; i < 20; i++)
    m->matx[i] &= col_masks20x20[line];
  
  return 0;
}
