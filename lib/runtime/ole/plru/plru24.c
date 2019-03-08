#include "plru24.h"

//These are accessed by triplets, OK ?!
dword line_mask24[24] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080,
			  0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
			  0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000 };
			 
//Check for ARCH
#if __x86_64
inline int find_first_zero24(u_int bit_array)
{
  u_int pos = 0;

  __asm__("bsfl %1,%0\n\t"
	  "jne 1f\n\t"
	  "movl $24, %0\n"
	  "1:"
	  : "=r" (pos)
	  : "r" (~(bit_array)));

  return (pos < 24) ? pos : -1;
}
#else
#warning "The actual architecture doesn't handle : BSFL\n"

#endif

//
void init_plru24(bit_list24 *l)
{ l->list = 0x00000000; }

//
int update_plru24(bit_list24 *l, u_int line)
{
  l->list |= line_mask24[line];
  
  //No zero found
  if (find_first_zero24(l->list) < 0)
    {
      l->list = 0x00000000;
      l->list |= line_mask24[line];
    }
  
  return 0;
}

//
int get_evict_line24(bit_list24 *l)
{ return find_first_zero24(l->list); }
