#include <stdio.h>

//
typedef unsigned u_int;
typedef unsigned char byte;

//[ 7 6 5 4 3 2 1 0 ]
typedef struct { byte list; } bit_list8;

int find_first_zero8(byte);
void init_plru8(bit_list8 *);
int get_evict_line8(bit_list8 *);
int update_plru8(bit_list8 *, u_int);
