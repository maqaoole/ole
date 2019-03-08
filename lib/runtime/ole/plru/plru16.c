#include "plru16.h"

//These are accessed by pairs OK ?! line_mask[i] & line_mask[i + 1] 
word line_mask16[16] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
			 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };

//Check for ARCH
#if __x86_64

FIND_FIRST_ZERO_DEF(16)

#else
#warning "The actual architecture doesn't handle BSFL instruction !\n"
#endif

//
void init_plru16(bit_list16 *l)
{ l->list = 0x0000; }

//
int update_plru16(bit_list16 *l, u_int line)
{
  l->list |= line_mask16[line];
  
  //No zero found
  if (find_first_zero_16_(l->list) < 0)
    {
      l->list = 0x0000;
      l->list |= line_mask16[line];
    }
}

//
int get_evict_line16(bit_list16 *l)
{ return find_first_zero_16_(l->list); }
