#include <stdio.h>

#define NB_LINES 12

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

/*
  12 lines x 16 columns = 192 bits
  Only 144 re needed, thus 48 bits are wasted !
  
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]
  [ ................ ]

 */

//LRU 12way
typedef struct { u_short matx[NB_LINES]; } refmatx144;

void init_zero144(refmatx144 *);
int  get_zero_pos144(refmatx144 *);
void print_LRU_matx144(refmatx144 *);
int  update_LRU_line144(refmatx144 *, u_int);
