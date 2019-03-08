#include "plru12.h"

//
word line_mask12[12] = { 0x0001, 0x0002, 0x0004, 0x0008, 
			 0x0010, 0x0020, 0x0040, 0x0080,
			 0x0100, 0x0200, 0x0400, 0x0800 };

//Check for ARCH
#if __x86_64
inline int find_first_zero12(unsigned bit_array)
{
  u_int pos = 0;

  __asm__("bsfl %1,%0\n\t"
	  "jne 1f\n\t"
	  "movl $32, %0\n"
	  "1:"
	  : "=r" (pos)
	  : "r" (~(bit_array)));

  return (pos < 12) ? pos : -1;
}
#else
#warning "The actual architecture doesn't handle : BSFL\n"

#endif

//
void init_plru12(bit_list12 *l)
{ l->list = 0x0000; }

//
int update_plru12(bit_list12 *l, u_int line)
{
  l->list |= line_mask12[line];
  
  //No zero found
  if (find_first_zero12(l->list) < 0)
    {
      l->list = 0x0000;
      l->list |= line_mask12[line];
    }
}

//
int get_evict_line12(bit_list12 *l)
{ return find_first_zero12(l->list); }
