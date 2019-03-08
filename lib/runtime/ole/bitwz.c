#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

//Convert a 4 char string to an int 
unsigned str2hex(char *str)
{
  char *p = str;
  unsigned x = 0x00000000;
  
  while (*p)
    x <<= 8, x |= *(p++);
  
  return x;
}

//Convert a 4-byte code into a 4 char string
char *hex2str(unsigned code)
{
  char *tmp = (char *)malloc(sizeof(char) * 5);

  tmp[0] = code >> 24;
  tmp[1] = ((code << 8) >> 24);
  tmp[2] = ((code << 16) >> 24);
  tmp[3] = ((code << 24) >> 24);
  tmp[4] = '\0';   
  
  return tmp;
}

//The shitty 'mathematical' way ...
uint64_t lg2(uint64_t n)
{ return (uint64_t)(log((double)n) / log(2.0)); }
