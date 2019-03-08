#include <stdio.h>

//
typedef unsigned u_int;
typedef unsigned short word;

//
typedef struct { word list; } bit_list12;

int find_first_zero12(unsigned);
void init_plru12(bit_list12 *);
int get_evict_line12(bit_list12 *);
int update_plru12(bit_list12 *, u_int);
