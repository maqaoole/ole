#include <stdio.h>

//
typedef unsigned u_int;
typedef u_int dword;

//
typedef struct { dword list; } bit_list32;

int find_first_zero32(unsigned);
void init_plru32(bit_list32 *);
int get_evict_line32(bit_list32 *);
int update_plru32(bit_list32 *, u_int);
