#include "lru16.h"

//These are used to set columns to 0 & keep the other bits intact !
unsigned long long col_masks256[8] = { 0x7F7F7F7F7F7F7F7F,   //line0 
                                       0xBFBFBFBFBFBFBFBF,   //line1
				       0xDFDFDFDFDFDFDFDF,   //line2
				       0xEFEFEFEFEFEFEFEF,   //line3
				       0xF7F7F7F7F7F7F7F7,   //line4
				       0xFBFBFBFBFBFBFBFB,   //line5
				       0xFDFDFDFDFDFDFDFD,   //line6
				       0xFEFEFEFEFEFEFEFE }; //line7 

//
unsigned long long row_masks256 = 0xFF00000000000000;

//
void init_zero256(refmatx256 *m)
{ 
  //Zero it all :D
  m->matx0 = 0x0000000000000000; m->matx1 = 0x0000000000000000;
  m->matx2 = 0x0000000000000000; m->matx3 = 0x0000000000000000;
}

//
u_char get_byte256(unsigned long long a, int pos)
{ return (a << (pos * 8)) >> (7 * 8); }

//
int get_zero_pos256(refmatx256 *m)
{
  for (int i = 0; i < 8; i++)
    if (get_byte256(m->matx0, i) == 0 && get_byte256(m->matx1, i) == 0)
      return i;
  
  for (int i = 0; i < 8; i++)
    if (get_byte256(m->matx2, i) == 0 && get_byte256(m->matx3, i) == 0)
      return i + 8;
}

//When cache line i is accessed : (m->matx | row_mask >> i) & col_mask[i]; 
//Line goes from 1 to 16
int update_LRU_line256(refmatx256 *m, u_int line)
{ 
  if (line < 8) //0 to 7
    {       
      m->matx0 = (m->matx0 | row_masks256 >> (line * 8)) & col_masks256[line]; 
      m->matx1 = (m->matx1 | row_masks256 >> (line * 8));		  
      m->matx2 = (m->matx2 & col_masks256[line]); 
    }
  else //8 to 15 
    { 
      m->matx3 = (m->matx3 | row_masks256 >> ((line - 8) * 8)) & col_masks256[line - 8];
      m->matx2 = (m->matx2 | row_masks256 >> ((line - 8) * 8));
      m->matx1 = (m->matx1 & col_masks256[line - 8]);
    }
  
  return 0;
}

//
void print_LRU_matx256(refmatx256 *m)
{
  printf("0x%016llx 0x%016llx\n"
	 "0x%016llx 0x%016llx\n", m->matx0, m->matx1, 
	                          m->matx2, m->matx3); 
}
