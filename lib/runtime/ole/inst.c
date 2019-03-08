#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include "init.h"
#include "structs.h"
#include "inst.h"

//
uint64_t OLE_get_nb_instructions(_OLE_loop_t *loop)
{
  uint64_t nb_inst = 0;
  _OLE_instruction_t *tmp = loop->ll_instructions;
  
  while (tmp)
    {
      nb_inst++;
      tmp = tmp->next;
    }
  
  return nb_inst;
}

//Return NULL if not found & the instruction's address if found
_OLE_instruction_t *OLE_lookup_instruction(_OLE_instruction_t *root, uint64_t key)
{
  _OLE_instruction_t *tmp = root;
  
  while (tmp && (tmp->bin_address != key))
    tmp = tmp->next;
  
  return tmp;
}


//Add a new instruction at the end of the LL
//Return instruction address if success, NULL if fail
_OLE_instruction_t *OLE_new_instruction(_OLE_instruction_t **root, uint64_t bin_addr, unsigned char type, uint64_t nb_caches, 
					/* uint64_t stream_id, */ char *asmline, uint64_t iter)
{
  _OLE_instruction_t *tmp = *root, *new_inst = (_OLE_instruction_t *)malloc(sizeof(_OLE_instruction_t));
  
  new_inst->iter = 0;
  new_inst->type = type;
  new_inst->next = NULL;
  new_inst->lstatus = '-';
  new_inst->fstatus = '-';
  new_inst->asmline = asmline;
  new_inst->stats.RAM_access = 0;
  new_inst->bin_address = bin_addr;
  /* new_inst->stream_id = stream_id; */
  
  if (!iter)
    {
      new_inst->stats.lst_stats = (_OLE_stats_t *)malloc(sizeof(_OLE_stats_t) * nb_caches);
      
      //Zeroing up ... Technically, this shouldn't be necessary !
      for (uint64_t i = 0; i < nb_caches; i++)
	{
	  new_inst->stats.lst_stats[i].hit = 0;
	  new_inst->stats.lst_stats[i].cold_miss = 0;
	  new_inst->stats.lst_stats[i].conf_miss = 0;
	}
    }

  //Reach the last node of the LL
  if (tmp)
    {
      while (tmp->next)
	tmp = tmp->next;
      
      tmp->next = new_inst;
      
      return tmp->next;
    }
  else
    {
      *root = new_inst; //if root == NULL

      return *root;
    }
}

