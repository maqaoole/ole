#include <stdio.h>

//
typedef unsigned u_int;
typedef unsigned char byte;

//[ 7 6 5 4 3 2 1 0 ]
typedef struct { byte list; } bit_list4;

int find_first_zero4(byte);
void init_plru4(bit_list4 *);
int get_evict_line4(bit_list4 *);
int update_plru4(bit_list4 *, u_int);
