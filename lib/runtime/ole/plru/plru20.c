#include "plru20.h"

//
dword line_mask20[20] = { 0x00000001, 0x00000002, 0x00000004, 0x00000008,
			  0x00000010, 0x00000020, 0x00000040, 0x00000080,
			  0x00000100, 0x00000200, 0x00000400, 0x00000800, 
			  0x00001000, 0x00002000, 0x00004000, 0x00008000,
			  0x00010000, 0x00020000, 0x00040000, 0x00080000 };
			 

int bitcount(u_int bit_array)
{
  u_int count;

  count = bit_array - ((bit_array >> 1) & 033333333333) - ((bit_array >> 2) & 011111111111);
  
  return ((count + (count >> 3)) & 030707070707) % 63;
}

//
inline int find_first_zero20(u_int bit_array)
{
  bit_array = ~bit_array;
  
  return bitcount((bit_array & (-bit_array)) - 1);
}

//
void init_plru20(bit_list20 *l)
{ l->list = 0x00000000; }

//
int update_plru20(bit_list20 *l, u_int line)
{
  l->list |= line_mask20[line];
  
  //No zero found
  if (find_first_zero24(l->list) < 0)
    {
      l->list = 0x00000000;
      l->list |= line_mask20[line];
    }
  
  return 0;
}

//
int get_evict_line20(bit_list20 *l)
{ return find_first_zero20(l->list); }
