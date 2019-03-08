#include "plru8.h"

//Cannot use shifts, must conserve the previous bits' state
byte line_mask8[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

/*

  Valid only on x86 with bsfl instruction !
  If you can't get what this function does,
  you should go flip burgers rather than code :]

  Yaspr.

*/
#if __x86_64
inline int find_first_zero8(byte bit_array)
{
  u_int pos = 0;

  __asm__("bsfl %1,%0\n\t"
	  "jne 1f\n\t"
	  "movl $32, %0\n"
	  "1:"
	  : "=r" (pos)
	  : "r" (~(bit_array)));
  
  return (pos > 7) ? -1 : pos;
}
#else
#warning "The actual architecture doesn't handle : BSFL\n"

#endif

//
void init_plru8(bit_list8 *l)
{ l->list = 0x00; }

//
int update_plru8(bit_list8 *l, u_int line)
{
  l->list |= line_mask8[line];
  
  //No zero found
  if (find_first_zero8(l->list) < 0)
    {
      l->list = 0x00;
      l->list |= line_mask8[line];
    }
}

//
int get_evict_line8(bit_list8 *l)
{ return find_first_zero8(l->list); }

