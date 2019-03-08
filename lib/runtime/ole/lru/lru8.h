#include <stdio.h>

//Tests if a word contains a zero byte. 
//For larger words (64bits, 126bits) use larger constants.
#define haszero32(a) (((a) - 0x01010101UL) & ~(a) & 0x80808080UL)

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

/*
  Sizeof(unsigned long long) = 8 Bytes = 64 bits, that's the 
  amount of bits needed for the reference matrix for a 8way cache !
 
  Here's what the reference matrix will look like :
  [ ........ ] --> is a line with 8 columns (each column is a bit)

  Line 0 [ ........ ]
  Line 1 [ ........ ]
  Line 2 [ ........ ]
  Line 3 [ ........ ]    
  Line 4 [ ........ ]   
  Line 5 [ ........ ] 
  Line 6 [ ........ ]
  Line 7 [ ........ ]

  0x0011223344556677
    ^       ^
    |       |
   Line0   Line4
   
   When a line (a byte) is equal to zero, that's the LRU line
*/

//LRU 8way
typedef struct { unsigned long long matx; } refmatx64;

void   init_zero64(refmatx64 *);
int    get_zero_pos64(refmatx64 *);
void   print_LRU_matx64(refmatx64 *);
u_char get_byte64(unsigned long long, int);
void   update_LRU_line64(refmatx64 *, u_int);
