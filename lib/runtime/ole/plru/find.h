/*

  This is reusability : Oh Yeah !
  If you learn something from this, good.
  If you can't understand what this is for, well, McDonald's 
  and Starbucks are always hiring. 
 
*/

#define FIND_FIRST_ZERO_PRO(EXT) int find_first_zero_##EXT##_(unsigned);

#define FIND_FIRST_ZERO_DEF(EXT)				\
  inline int find_first_zero_##EXT##_(unsigned bit_array)	\
  {								\
    u_int pos = 0;						\
    								\
    __asm__("bsfl %1,%0\n\t"					\
	    "jne 1f\n\t"					\
	    "movl $32, %0\n"					\
	    "1:"						\
	    : "=r" (pos)					\
	    : "r" (~(bit_array)));				\
    								\
    return (pos < EXT) ? pos : -1;				\
  }                                                        
