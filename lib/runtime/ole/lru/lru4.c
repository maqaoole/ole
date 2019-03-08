#include "lru4.h"

//These are used to set columns to 0 & keep the other bits intact !
u_short col_masks16[4] = { 0x7777,   //line0 
			   0xBBBB,   //line1
			   0xDDDD,   //line2
			   0xEEEE }; //line3

//
u_short row_masks16 = 0xF000;

//Zero it all :D
void init_zero16(refmatx16 *m)
{ m->matx = 0x0000; }

//Hand dichotomy - must find a no branch bit-wise way to do it !! 
int get_zero_pos16(u_short a)
{
  u_char l = (a >> 8), r = (a << 8) >> 8;
  u_char llz = l >> 4, lrz = l ^ (l >> 4) << 4, rlz = r >> 4;
  
  return (llz == 0) ? 0 : (lrz == 0) ? 1 : (rlz == 0) ? 2 : 3;
}

//When cache line i is accessed : (m->matx | row_mask >> i) & col_mask[i]; 
void update_LRU_line16(refmatx16 *m, u_int line)
{ m->matx = (m->matx | row_masks16 >> (line * 4)) & col_masks16[line]; }

//
void print_LRU_matx16(refmatx16 *m)
{ printf("0x%04x\n", m->matx); }
