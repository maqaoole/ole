#include "lru24.h"

//Done manually (Why ? It's faster !)
unsigned col_masks24x24[24] = { 0x00FFFFFE, 0x00FFFFFD, 0x00FFFFFB, 0x00FFFFF7, 
				0x00FFFFEF, 0x00FFFFDF, 0x00FFFFBF, 0x00FFFF7F,
				0x00FFFEFF, 0x00FFFDFF, 0x00FFFBFF, 0x00FFF7FF,
				0x00FFEFFF, 0x00FFDFFF, 0x00FFBFFF, 0x00FF7FFF,
				0x00FEFFFF, 0x00FDFFFF, 0x00FBFFFF, 0x00F7FFFF,
				0x00EFFFFF, 0x00DFFFFF, 0x00BFFFFF, 0x007FFFFF };

//
int get_zero_pos24x24(refmatx24x24 *m)
{
  for (int i = 0; i < 24; i++)
    if (!m->matx[i])
      return i;
}

//
int update_LRU_line24x24(refmatx24x24 *m, u_int line)
{
  m->matx[line] = 0x000FFFFF;
  
  for (int i = 0; i < 24; i++)
    m->matx[i] &= col_masks24x24[line];
  
  return 0;
}
