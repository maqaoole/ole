#include <stdio.h>

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

//LRU 32way
typedef struct { u_int matx[32]; } refmatx1024;

void init_zero1024(refmatx1024 *);
int  get_zero_pos1024(refmatx1024 *);
void print_LRU_matx1024(refmatx1024 *);
int  update_LRU_line1024(refmatx1024 *, u_int);
