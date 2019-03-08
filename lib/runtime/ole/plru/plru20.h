#include <stdio.h>

//
typedef unsigned u_int;
typedef u_int dword;

//
typedef struct { dword list; } bit_list20;

int find_first_zero20(dword);
void init_plru20(bit_list20 *);
int get_evict_line20(bit_list20 *);
int update_plru20(bit_list20 *, u_int);
