#include <lua.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <lualib.h>
#include <unistd.h>
#include <lauxlib.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <pthread.h>

#include "lru.h"
#include "init.h"
#include "dump.h"
#include "inst.h"
#include "trace.h"

#include "ole.h"

//
int squash(int v)
{
  return v % global_config->nb_cores;
}

//
int lookup_free_cacheline(_OLE_cacheline_t *cachelines, uint64_t assoc)
{
  unsigned i = 0;
  
  //While cachelines are used keep looking
  while (cachelines[i].state == 'u' && i < assoc) 
    i++;
  
  return i; //if i == assoc then no free cacheline found ==> EVICTION 
}

//Cuts an address into TAG SET OFFSET using the *_LEN 
void slice_address(_OLE_address_t *a, uint64_t LEN_TAG, uint64_t LEN_SET, uint64_t LEN_OFF)
{  
  a->TAG = (a->addr) >> (LEN_SET + LEN_OFF);
  a->SET = ((a->addr) << LEN_TAG) >> (LEN_TAG + LEN_OFF);
  a->OFF = ((a->addr) << (LEN_TAG + LEN_SET)) >> (LEN_TAG + LEN_SET);
}

//Inserts address starting level l taking into account eviction 
int update(_OLE_core_t *core, _OLE_address_t *a, uint64_t l, _OLE_loop_t *loop, uint64_t fret, uint64_t baddr)
{
  int line = 0;
  uint64_t i;
  unsigned found;
  _OLE_set_t *set;
  _OLE_cache_t *cache;
  
  //Update replacement status 
  for (i = 0; i < l; i++)
    {      
      cache = &core->lst_caches[i];

      slice_address(a, cache->LEN_TAG, cache->LEN_SET, cache->LEN_OFF);

      set = &cache->lst_sets[a->SET];

      if (DBG_MODE)
	printf("  UPDATING: %" PRIu64 "\n", i);

      //Look up free cacheline
      found = lookup_free_cacheline(set->lst_cachelines, cache->assoc);
      
      //If free cacheline found in set ==> line = cacheline position  
      //Else conflict miss ==> eviction & line = evicted line position
      
      if (fret == CONF_MISS)
	{	  
	  //Get line index
	  line = cache->rep_pol_line_fct_ptr(cache->lst_sets[a->SET].rep_pol_var);

      	  //Evict the cacheline
	  cache->rep_pol_fct_ptr(cache->lst_sets[a->SET].rep_pol_var, line);	  
	  
	  if (DBG_MODE)
	    printf("EVICT: 0x%" PRIx64 "\n", cache->lst_sets[a->SET].lst_cachelines[line].addr.addr);

	  //
	  cache->lst_sets[a->SET].lst_cachelines[line].owner_addr     = baddr;
	  cache->lst_sets[a->SET].lst_cachelines[line].owner_memlev   = cache->level - 1;;
	  cache->lst_sets[a->SET].lst_cachelines[line].owner_instance = nb_instances;
      
	  cache->lst_sets[a->SET].linesin++;
	} 
      else
	{	  
	  if (found < cache->assoc) //Available space in the cache
	    line = found;

	  if (fret == COLD_MISS)
	    {
	      cache->lst_sets[a->SET].lst_cachelines[line].owner_addr     = baddr;
	      cache->lst_sets[a->SET].lst_cachelines[line].owner_memlev   = cache->level - 1;;
	      cache->lst_sets[a->SET].lst_cachelines[line].owner_instance = nb_instances;
	    }
	}
      
      //
      cache->lst_sets[a->SET].lst_cachelines[line].state = 'u'; //f for free, u for used
      
      //Cacheline alignment --> address offset bits must be set to 0
      cache->lst_sets[a->SET].lst_cachelines[line].addr.addr = a->addr; //(a->addr >> cache->LEN_OFF) << cache->LEN_OFF; 
      cache->lst_sets[a->SET].lst_cachelines[line].addr.TAG  = a->TAG;
      
      cache->lst_sets[a->SET].linesin += (fret != CONF_MISS);
      
      //Set owner of the cacheline 
      cache->lst_sets[a->SET].lst_cachelines[line].owner_coreID = core->coreID;      
    }

  if (fret == COLD_MISS)
    if (nb_instances < MAX_INSTANCES) //Handle a limited nmber of instances 
      {
	if (loop->iter < MAX_ITER)  //Handle a limited number of iterations
	  {
	    uint64_t tmp_nb_insn = loop->lst_groups[loop->iter].nb_instructions;
	    
	    if (tmp_nb_insn < MAX_INSN)
	      {	      	      
		loop->lst_groups[loop->iter].
		  lst_instructions[tmp_nb_insn].
		  cacheline_address = a->addr; 

		loop->lst_groups[loop->iter].
		  lst_instructions[tmp_nb_insn].
		  cacheline_offset = a->OFF;

		loop->lst_groups[loop->iter].
		  lst_instructions[tmp_nb_insn].
		  bin_addr = baddr;

		loop->lst_groups[loop->iter].
		  lst_instructions[tmp_nb_insn].
		  iter = loop->iter;

		loop->lst_groups[loop->iter].
		  lst_instructions[tmp_nb_insn].
		  memlev = l;
		
		loop->nb_groups = loop->iter;
		loop->lst_groups[loop->iter].nb_instructions++;
	      }
	  }
      }
  
  return line;
}

//Searches for an address in any cache level
/*
  hit
  miss : cold, conflict
*/
int fetch(_OLE_cache_t *cache, _OLE_address_t *a, _OLE_cacheline_t **cl)
{  
  uint64_t found = 0, i;
  _OLE_cacheline_t *tmp = NULL;
  _OLE_set_t *set = &cache->lst_sets[a->SET];
  
  //Going through the cache lines  
  for (i = 0; !found && i < cache->assoc; i++)
    {      
      tmp = &set->lst_cachelines[i];
      
      //If TAG found, cacheline loaded 
      found = (a->TAG == tmp->addr.TAG);
    }
  
   (*cl) = tmp; //Return the owner of the cache line for grouping
     
   if (found)
     return HIT;
   else
     if (set->linesin < cache->assoc) //Set not full --> COLD_MISS
       {
	 (*cl ) = tmp;
	 return COLD_MISS;
       }
     else //Set full --> CONF_MISS (eviction must happen)
       return CONF_MISS;
}

//Print LL instruction and run instruction
int OLE_inject_cacheline_(_OLE_core_t *core, uint64_t nb_caches, _OLE_loop_t *loop, uint64_t baddr, 
			  unsigned char inst_type, /* uint64_t stream_id, */ char *asmline, _OLE_address_t *iaddr)
{
  uint64_t j, line;
  _OLE_cacheline_t *cl;
  _OLE_instruction_t *inst;
  uint64_t fret = COLD_MISS;
  _OLE_cache_t *cache = &core->lst_caches[0]; //Single core!
  
  //Look up the instruction in the LL, if !exists add it to the end of the LL
  if ((inst = OLE_lookup_instruction(loop->ll_instructions, baddr)) == NULL)
    inst = OLE_new_instruction(&loop->ll_instructions, baddr, inst_type, nb_caches, /* stream_id, */ asmline, 0);      
  
  //Collect addresses for pattern analysis
  if (!(inst->paxt_done))
    {
      inst->addresses[inst->nb_addresses++] = iaddr->addr;
      
      if (inst->nb_addresses == MAX_ADDRESSES)
	inst->paxt_done = 1;
    }
    
  //If no hit ==> keep looking in lower level caches
  for (j = 0; (fret == COLD_MISS || fret == CONF_MISS)  && j < nb_caches; j++)
    {     
      cache = &core->lst_caches[j];
      
      slice_address(iaddr, cache->LEN_TAG, cache->LEN_SET, cache->LEN_OFF);	    
      
      fret = fetch(cache, iaddr, &cl);
      
      if (DBG_MODE)
	printf("INST: %" PRIu64 " ITER: %" PRIu64 " INSN: 0x%" PRIx64 " MEM: 0x%" PRIx64 " (%10" PRIu64 ", %10" PRIu64 ", %10" PRIu64 ") %s %s\n",
	       nb_instances, loop->iter, baddr, iaddr->addr, iaddr->TAG, iaddr->SET, iaddr->OFF, _dbg_events[fret], _dbg_levels[cache->level - 1]);
      
      //
      /* if (fret == CONF_MISS) */
      /* 	{ */
      /* 	  //Get line index */
      /* 	  line = cache->rep_pol_line_fct_ptr(cache->lst_sets[iaddr->SET].rep_pol_var); */
	  
      /* 	  //Evict the cacheline */
      /* 	  cache->rep_pol_fct_ptr(cache->lst_sets[iaddr->SET].rep_pol_var, line);	   */
	  
      /* 	  if (DBG_MODE) */
      /* 	    printf("EVICT: 0x%" PRIx64 "\n", cache->lst_sets[iaddr->SET].lst_cachelines[line].addr.addr); */
	  
      /* 	  cache->lst_sets[iaddr->SET].linesin++; */
	  
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].state = 'u'; //f for free, u for used */
	  
      /* 	  //Cacheline alignment --> address offset bits must be set to 0 */
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].addr.addr = iaddr->addr; //(iaddr->addr >> cache->LEN_OFF) << cache->LEN_OFF;  */
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].addr.TAG  = iaddr->TAG; */
	  
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].owner_addr     = baddr; */
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].owner_memlev   = cache->level - 1; */
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].owner_instance = nb_instances; */
	  
      /* 	  cl = &cache->lst_sets[iaddr->SET].lst_cachelines[line]; */
	  
      /* 	  //Set owner of the cacheline  */
      /* 	  cache->lst_sets[iaddr->SET].lst_cachelines[line].owner_coreID = core->coreID; */
      /* 	} */
      
      inst->stats.lst_stats[cache->level - 1].hit       += (fret == HIT);
      inst->stats.lst_stats[cache->level - 1].cold_miss += (fret == COLD_MISS);
      inst->stats.lst_stats[cache->level - 1].conf_miss += (fret == CONF_MISS);
    }
  
  //Handle grouping
  if (fret == HIT)
    {
      if (nb_instances < MAX_INSTANCES)
	{
	  //cacheline -> lst_groups(iter) -> lst_insns      
	  if (loop->iter < MAX_ITER) 
	    {
	      uint64_t tmp_nb_insn = loop->lst_groups[loop->iter].nb_instructions;
	      
	      if (tmp_nb_insn < MAX_INSN)
		{	      	      
		  loop->lst_groups[loop->iter].
		    lst_instructions[tmp_nb_insn].
		    cacheline_address = iaddr->addr;

		  loop->lst_groups[loop->iter].
		    lst_instructions[tmp_nb_insn].
		    cacheline_offset = iaddr->OFF; 

		  loop->lst_groups[loop->iter].
		    lst_instructions[tmp_nb_insn].
		    bin_addr = baddr;

		  loop->lst_groups[loop->iter].
		    lst_instructions[tmp_nb_insn].
		    iter = loop->iter;

		  loop->lst_groups[loop->iter].
		    lst_instructions[tmp_nb_insn].
		    memlev = cache->level;
		  
		  loop->nb_groups = loop->iter;
		  loop->lst_groups[loop->iter].nb_instructions++;
		}
	    }
	}

      if (cl)
	inst->laddr = cl->owner_addr;
    }
  
  //If miss then load cacheline from lower to upper levels 
  //If hit on any memory level but L1 then load cacheline from that level up
  //L1 hit ==> nothing to be done!
  if (fret == COLD_MISS || fret == CONF_MISS || (fret == HIT && cache->level != 1))
    {
      //The instruction that causes a cold miss is
      //automatically the owner of the cacheline untill eviction
      if (fret == COLD_MISS || fret == CONF_MISS)
	{
	  inst->laddr = baddr;
	}
      
      if (DBG_MODE)
	printf("UPDATE: INSN: 0x%" PRIx64 " LVL: %" PRIu64 " EVENT: %s MEM: 0x%" PRIx64 "\n", baddr, cache->level, _dbg_events[fret], iaddr->addr);

      //Update all levels up
      update(core, iaddr, cache->level, loop, fret, baddr);
    }
  
  return 0;
}

//
void OLE_inject_cacheline(uint64_t loopID, uint64_t baddr, unsigned inst, /* uint64_t stream_id, */ char *asmline, uint64_t iaddr)
{
  _OLE_address_t a;
  
  a.addr = iaddr;
  
  //Handle parallel execution for this version in the future (runtime agnostic?!) 
#ifdef SYS_gettid
  pid_t tid = syscall(SYS_gettid);
#else
#error "SYS_gettid unavailable on this system"
#endif
  
  OLE_inject_cacheline_(&global_cpu->lst_cores[squash(tid)], 
			global_cpu->nb_caches, global_tmp_loop, baddr, (inst) ? 'l' : 's', 
			/* stream_id, */ asmline, &a); 
  
  //Dump once when the program reaches the end nothing has been dumped before
  //OLE_exit();
  
  nb_calls++;  
}
