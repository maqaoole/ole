#include <stdint.h>
#include <stdlib.h>

#include "structs.h"
#include "trace.h"

//
_OLE_trace_t *OLE_trace_init()
{ return NULL; }

//
_OLE_trace_t *OLE_lookup_trace_address(_OLE_trace_t *root, uint64_t addr)
{
  _OLE_trace_t *tmp = root;
  
  //While address is not found keep going through the LL
  while (tmp && (tmp->addr != addr))
    tmp = tmp->next;
  
  //If not found return NULL else return pointer to location
  return tmp;
}

//
_OLE_trace_meta_t *OLE_add_new_meta_entry(_OLE_trace_meta_t **node, uint64_t bin_addr, uint64_t iter)
{
  _OLE_trace_meta_t *tmp = *node;
  
  //Till the end of the LL, tmp points at the last node
  while (tmp && tmp->next)
    tmp = tmp->next;
  
  if (tmp)
    tmp = tmp->next = malloc(sizeof(_OLE_trace_meta_t));
  else
    *node = tmp = malloc(sizeof(_OLE_trace_meta_t));
  
  tmp->bin_addr = bin_addr;
  tmp->iter = iter;
  tmp->next = NULL;
  
  return tmp;
}

//
_OLE_trace_t *OLE_add_new_trace_entry(_OLE_trace_t **root, uint64_t addr, uint64_t bin_addr, uint64_t iter)
{
  _OLE_trace_t *tmp = *root;
  
  //Till the end of the LL, tmp points at the last node
  while (tmp && tmp->next)
    tmp = tmp->next;
  
  if (tmp)
    tmp = tmp->next = malloc(sizeof(_OLE_trace_t));
  else
    *root = tmp = malloc(sizeof(_OLE_trace_t));
  
  tmp->addr = addr;
  tmp->next = NULL;
  
  //Adding meta info ...
  OLE_add_new_meta_entry(&tmp->meta, bin_addr, iter);
  
  return tmp;
}

