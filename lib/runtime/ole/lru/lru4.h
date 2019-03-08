#include <stdio.h>

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned u_int;

//LRU 4way
typedef struct { u_short matx; } refmatx16;

int get_zero_pos16(u_short);
void init_zero16(refmatx16 *);
void print_LRU_matx16(refmatx16 *);
void update_LRU_line16(refmatx16 *, u_int);
