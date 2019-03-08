#include "lru8.h"

//These are used to set columns to 0 & keep the other bits intact !
unsigned long long col_masks64[8] = { 0x7F7F7F7F7F7F7F7F,   //line0 
                                      0xBFBFBFBFBFBFBFBF,   //line1
				      0xDFDFDFDFDFDFDFDF,   //line2
				      0xEFEFEFEFEFEFEFEF,   //line3
				      0xF7F7F7F7F7F7F7F7,   //line4
				      0xFBFBFBFBFBFBFBFB,   //line5
				      0xFDFDFDFDFDFDFDFD,   //line6
				      0xFEFEFEFEFEFEFEFE }; //line7  

//
unsigned long long row_masks64 = 0xFF00000000000000;

//Zero it all :D
void init_zero64(refmatx64 *m)
{ m->matx = 0x0000000000000000; }

//
u_char get_byte64(unsigned long long a, int pos)
{ return (a << (pos * 8)) >> (7 * 8); }

//Hand dichotomy - must find a no branch bit-wise way to do it !! 
int get_zero_pos64(refmatx64 *m)
{
  for (int i = 0; i < 8; i++)
    if (!get_byte64(m->matx, i))
      return i;
}

//When cache line i is accessed : (m->matx | row_mask >> i) & col_mask[i]; 
void update_LRU_line64(refmatx64 *m, u_int line)
{ m->matx = (m->matx | row_masks64 >> (line * 8)) & col_masks64[line]; }

//
void print_LRU_matx64(refmatx64 *m)
{ printf("0x%016llx\n", m->matx); }
