#include "plru4.h"

//Cannot use shifts, must conserve the previous bits' state
byte line_mask4[4] = { 0x01, 0x02, 0x04, 0x08 };

/*

  Valid only on x86 with bsfl instruction !
  If you can't get what this function does,
  you should go flip burgers rather than code :]

  Yaspr
  
*/
#if __x86_64
inline int find_first_zero4(byte bit_array)
{
  u_int pos = 0;
  
  __asm__("bsfl %1,%0\n\t"
	  "jne 1f\n\t"
	  "movl $32, %0\n"
	  "1:"
	  : "=r" (pos)
	  : "r" (~(bit_array)));
  
  return (pos < 4 ) ? pos : -1;
}
#else
#warning "The actual architecture doesn't handle : BSFL\n"

#endif

//
void init_plru4(bit_list4 *l)
{ l->list = 0x00; }

//
int update_plru4(bit_list4 *l, u_int line)
{
  l->list |= line_mask4[line];
  
  //No zero found
  if (find_first_zero4(l->list) < 0)
    {
      l->list = 0x00;
      l->list |= line_mask4[line];
    }
}

//
int get_evict_line4(bit_list4 *l)
{ return find_first_zero4(l->list); }
