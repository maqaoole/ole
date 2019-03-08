#include <stdio.h>

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

//LRU 24way
typedef struct { u_int matx[32]; } refmatx24x24;

void init_zero24x24(refmatx24x24 *);
int  get_zero_pos24x24(refmatx24x24 *);
int  update_LRU_line24x24(refmatx24x24 *, u_int);
