#include "lru12.h"

//Only need 12 bits, hence the zero in the end !
u_short col_masks144[12] = { 0x7FF0, 0xBFF0, 0xDFF0, 0xEFF0,
				   0xF7F0, 0xFBF0, 0xFDF0, 0xFEF0,
				   0xFF70, 0xFFB0, 0xFFD0, 0xFFE0 };

//
void init_zero144(refmatx144 *m)
{
  //Vectorizable
  for (int i = 0; i < 12; i++)
    m->matx[i] = 0x0000;
}

//
int get_zero_pos144(refmatx144 *m)
{
  for (int i = 0; i < 12; i++)
    if (!m->matx[i])
      return i;
}

//
int update_LRU_line144(refmatx144 *m, u_int line)
{
  m->matx[line] = 0xFFF0; 
  
  for (int i = 0; i < 12; i++)
    m->matx[i] &= col_masks144[line];
  
  return 0;
}

//
void print_LRU_matx144(refmatx144 *m)
{
  for (int i = 0; i < 12; i++)
    printf("0x%x\n", (m->matx[i] >> 4));
}
