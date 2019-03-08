#include <stdio.h>

#include "find.h"

//
typedef unsigned u_int;
typedef unsigned short word;

//
typedef struct { word list; } bit_list16;

//int find_first_zero16(unsigned);
FIND_FIRST_ZERO_PRO(16)
void init_plru16(bit_list16 *);
int get_evict_line16(bit_list16 *);
int update_plru16(bit_list16 *, u_int);
