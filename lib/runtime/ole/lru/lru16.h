#include <stdio.h>

//Tests if a word contains a zero byte. 
//For larger words (64bits, 128bits) use larger constants.
#define haszero64(a) (((a) - 0x0101010101010101UL) & ~(a) & 0x8080808080808080UL)

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

/*
  Sizeof(unsigned long long) = 8 Bytes = 64 bits, 64 bits x4 = 256bits
  That's the amount of bits needed for the reference matrix for a 16way cache !
  
  Here's what the reference matrix will look like :
  [ ........ ] --> is a line with 8 columns

         Block1       Block2       Block1               Block2
  Line 0 [ ........ ] [ ........ ] 0x0011223344556677 0x7766554433221100 
  Line 1 [ ........ ] [ ........ ]   ^       ^          ^       ^
  Line 2 [ ........ ] [ ........ ]   |       |          |       |
  Line 3 [ ........ ] [ ........ ]  Line00  Line40     Line01  Line41 
  Line 4 [ ........ ] [ ........ ]  
  Line 5 [ ........ ] [ ........ ]
  Line 6 [ ........ ] [ ........ ]
  Line 7 [ ........ ] [ ........ ]
  
         Block3       Block4       Block3               Block4
  Line 8 [ ........ ] [ ........ ] 0x0011223344556677 0x7766554433221100 
  Line 9 [ ........ ] [ ........ ]   ^       ^          ^       ^
  Line10 [ ........ ] [ ........ ]   |       |          |       |
  Line11 [ ........ ] [ ........ ]  Line00  Line40     Line01  Line41 
  Line12 [ ........ ] [ ........ ]  
  Line13 [ ........ ] [ ........ ]
  Line14 [ ........ ] [ ........ ]
  Line15 [ ........ ] [ ........ ]

  When a line (a left block byte & a right block byte of the same position) is equal 
  to zero, that's the LRU line.
 */

//LRU 16way
typedef struct { unsigned long long matx0; unsigned long long matx1;
                 unsigned long long matx2; unsigned long long matx3; } refmatx256;

void   init_zero256(refmatx256 *);
int    get_zero_pos256(refmatx256 *);
void   print_LRU_matx256(refmatx256 *);
u_char get_byte256(unsigned long long, int);
int    update_LRU_line256(refmatx256 *, u_int);
