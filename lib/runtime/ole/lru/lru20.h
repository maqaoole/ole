#include <stdio.h>

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

//LRU 20way
typedef struct { u_int matx[32]; } refmatx20x20;

void init_zero20x20(refmatx20x20 *);
int  get_zero_pos20x20(refmatx20x20 *);
int  update_LRU_line20x20(refmatx20x20 *, u_int);
