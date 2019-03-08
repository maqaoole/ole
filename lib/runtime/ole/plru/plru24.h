#include <stdio.h>

//
typedef unsigned u_int;
typedef u_int dword;

//
typedef struct { dword list; } bit_list24;

int find_first_zero24(dword);
void init_plru24(bit_list24 *);
int get_evict_line24(bit_list24 *);
int update_plru24(bit_list24 *, u_int);
