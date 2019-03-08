#include <string.h>
#include <pthread.h>

#include "init.h"
#include "dump.h"
#include "consts.h"

#include "lru.h"
#include "plru.h"
#include "bitwz.h"

#define DIFF(a, b) (a > b) ? a - b : b - a

//Open and run the config lua file & return the lua state
lua_State *OLE_open_lua_config(char *fname)
{
  lua_State *L;
  int status, result;
  
  L = luaL_newstate();

  //luaL_openlibs(L);

  status = luaL_loadfile(L, fname);
  
  if (status) 
    return printf("Couldn't load file: %s\n", lua_tostring(L, -1)), NULL;
  
  result = lua_pcall(L, 0, LUA_MULTRET, 0);

  if (result) 
    return printf("Failed to run script: %s\n", lua_tostring(L, -1)), NULL;
  
  return L;
}

//Close the lua config file
int OLE_close_lua_config(lua_State *L)
{ 
  lua_close(L);
  
  return 0;
}

//Reads and returns an int
int OLE_lua_get_int_field(lua_State *L, const char *key) 
{
  int ret;
  
  lua_pushstring(L, key);
  lua_gettable(L, -2);
  
  ret = (int)lua_tonumber(L, -1);
  
  lua_pop(L, 1);

  return ret;
}

//Reads and returns an int
char *OLE_lua_get_str_field(lua_State *L, const char *key) 
{
  char *ret;
  
  lua_pushstring(L, key);
  lua_gettable(L, -2);
  
  ret = (char *)lua_tostring(L, -1);
  
  lua_pop(L, 1);

  return ret;
}

//When handling multi-cpu architectures
int OLE_get_socket_config()
{  return 0; }

//Read lua config file cpu parameters
int OLE_get_cpu_config(lua_State *L, _OLE_config_t *config)
{
  lua_getglobal(L, "nbcores");
  config->nb_cores = (uint64_t)lua_tonumber(L, -1);
  
  config->nb_caches = 3; //
  
  lua_pop(L, 1);

  return 0;
}
 
//Read lua config file core parameters
int OLE_get_core_config(lua_State *L, _OLE_config_t *config)
{
  char *rep_pol;
  uint64_t i = 0;
  const char *labels[] = { "L1", "L2", "L3" };
  
  lua_pushnil(L);
  
  while (i < 3)
    { 
      lua_getglobal(L, labels[i]);

      config->lst_caches[i].size     = OLE_lua_get_int_field(L, "size");
      config->lst_caches[i].cost     = OLE_lua_get_int_field(L, "cost");
      config->lst_caches[i].level    = OLE_lua_get_int_field(L, "level");
      config->lst_caches[i].assoc    = OLE_lua_get_int_field(L, "assoc");
      config->lst_caches[i].linesize = OLE_lua_get_int_field(L, "linesize");
     
      rep_pol = OLE_lua_get_str_field(L, "policy");
      config->lst_caches[i].rep_pol = str2hex(rep_pol); //Converting the 4-character string into its hex 4-byte representation 
      
      config->lst_caches[i].nb_sets = config->lst_caches[i].size / (config->lst_caches[i].linesize * config->lst_caches[i].assoc);
      
      config->lst_caches[i].LEN_SET = lg2(config->lst_caches[i].nb_sets);
      config->lst_caches[i].LEN_OFF = lg2(config->lst_caches[i].linesize);
      config->lst_caches[i].LEN_TAG = LEN_ADDR - config->lst_caches[i].LEN_SET - config->lst_caches[i].LEN_OFF;
      
      lua_pop(L, -1);
      
      i++;
    }
  
  lua_pop(L, 1);
  
  return 0;
}

//Read the full configuration file 
_OLE_config_t *OLE_load_config(char *cfgfname)
{
  _OLE_config_t *config = (_OLE_config_t *)malloc(sizeof(_OLE_config_t));
  lua_State *L = OLE_open_lua_config(cfgfname);
  
  //OLE_get_socket_config();
  OLE_get_cpu_config(L, config);
  OLE_get_core_config(L, config);
  
  OLE_close_lua_config(L);
  
  return config;
}

//Allocate and initialize replacement policy variable (reference matrix for LRU, ...)
void OLE_set_rep_pol_var(_OLE_set_t *set, unsigned rep_pol, int assoc)
{
  if (rep_pol == BLRU)
    {
      if (assoc == 4)
	{
	  set->rep_pol_var = (refmatx16 *)malloc(sizeof(refmatx16));
	  init_zero16(set->rep_pol_var);
	}
      else
	if (assoc == 8)
	  {
	    set->rep_pol_var = (refmatx64 *)malloc(sizeof(refmatx64));
	    init_zero64(set->rep_pol_var);
	  }
	else
	  if (assoc == 16)
	    {
	    set->rep_pol_var = (refmatx256 *)malloc(sizeof(refmatx256));
	    init_zero256(set->rep_pol_var);
	  }
	  else
	    if (assoc == 12)
	      {
		set->rep_pol_var = (refmatx144 *)malloc(sizeof(refmatx144));
		init_zero144(set->rep_pol_var);
	      }
	    else
	      if (assoc == 20)
		{
		  set->rep_pol_var = (refmatx20x20 *)malloc(sizeof(refmatx20x20));
		  init_zero1024(set->rep_pol_var);
		}
	      else
		if (assoc == 24)
		  {
		    set->rep_pol_var = (refmatx24x24 *)malloc(sizeof(refmatx24x24));
		    init_zero1024(set->rep_pol_var);
		  }
		else
		if (assoc == 32)
		  {
		    set->rep_pol_var = (refmatx1024 *)malloc(sizeof(refmatx1024));
		    init_zero1024(set->rep_pol_var);
		  }
		else
		  printf("Associativity level (%d) not handled yet\n", assoc), exit(0);
    }
  else if (rep_pol == PLRU)
    {
      if (assoc == 4)
	{
	  set->rep_pol_var = (bit_list4 *)malloc(sizeof(bit_list4));
	  init_plru4(set->rep_pol_var);
	}
      else
	if (assoc == 8)
	  {
	    set->rep_pol_var = (bit_list8 *)malloc(sizeof(bit_list8));
	    init_plru8(set->rep_pol_var);
	  }
	else
	  if (assoc == 12)
	    {
	      set->rep_pol_var = (bit_list12 *)malloc(sizeof(bit_list12));
	      init_plru12(set->rep_pol_var);
	    }
	else
	  if (assoc == 16)
	    {
	      set->rep_pol_var = (bit_list16 *)malloc(sizeof(bit_list16));
	      init_plru16(set->rep_pol_var);
	    }
	  else
	    if (assoc == 20)
	      {
		set->rep_pol_var = (bit_list20 *)malloc(sizeof(bit_list20));
		init_plru20(set->rep_pol_var);
	      }
	    else
	      if (assoc == 24)
		{
		  set->rep_pol_var = (bit_list24 *)malloc(sizeof(bit_list24));
		  init_plru24(set->rep_pol_var);
		}
	      else
		if (assoc == 32)
		  {
		    set->rep_pol_var = (bit_list32 *)malloc(sizeof(bit_list32));
		    init_plru32(set->rep_pol_var);
		  }
		else
		  printf("Associativity level (%d) not handled yet\n", assoc), exit(0);
    }
  else
    printf("Replacement policy not handled yet !\n"), exit(0);
}

//Set the replacement policy function pointer
void OLE_set_rep_pol_fct_ptr(_OLE_cache_t *cache, unsigned rep_pol, int assoc)
{
  if (rep_pol == BLRU)
    {
      if (assoc == 4)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line16;
	  cache->rep_pol_line_fct_ptr = get_zero_pos16;
	}
      else if (assoc == 8)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line64;
	  cache->rep_pol_line_fct_ptr = get_zero_pos64;
	}
      else if (assoc == 12)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line144;
      	  cache->rep_pol_line_fct_ptr = get_zero_pos144;
	}
      else if (assoc == 16)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line256;
      	  cache->rep_pol_line_fct_ptr = get_zero_pos256;
	}
      else if (assoc == 20)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line20x20;
      	  cache->rep_pol_line_fct_ptr = get_zero_pos20x20;
	}
      else if (assoc == 24)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line24x24;
      	  cache->rep_pol_line_fct_ptr = get_zero_pos24x24;
	}
      else if (assoc == 32)
	{
	  cache->rep_pol_fct_ptr = update_LRU_line1024;
	  cache->rep_pol_line_fct_ptr = get_zero_pos1024;
	}
    }
  else if (rep_pol == PLRU)
    {
      if (assoc == 4)
	{
	  cache->rep_pol_fct_ptr = update_plru4;
	  cache->rep_pol_line_fct_ptr = get_evict_line4;	  
	}
      else if (assoc == 8)
	{
	  cache->rep_pol_fct_ptr = update_plru8;
	  cache->rep_pol_line_fct_ptr = get_evict_line8;
	}
      else if (assoc == 12)
	{
	  cache->rep_pol_fct_ptr = update_plru12;
      	  cache->rep_pol_line_fct_ptr = get_evict_line12;
	}
      else if (assoc == 16)
	{
	  cache->rep_pol_fct_ptr = update_plru16;
	  cache->rep_pol_line_fct_ptr = get_evict_line16;
	}
      else if (assoc == 20)
	{
	  cache->rep_pol_fct_ptr = update_plru20;
	  cache->rep_pol_line_fct_ptr = get_evict_line20;
	}
      else if (assoc == 24)
	{
	  cache->rep_pol_fct_ptr = update_plru24;
	  cache->rep_pol_line_fct_ptr = get_evict_line24;
	}
      else if (assoc == 32)
	{
	  cache->rep_pol_fct_ptr = update_plru32;
	  cache->rep_pol_line_fct_ptr = get_evict_line32;
	} 
    }
}

//Build the configuration 
_OLE_cpu_t *OLE_init_(_OLE_config_t *config, uint64_t loopID)
{
  nb_calls = 0;
  registered_dump_call = 0;                                  //Init to 0 (not necessary for global variables)
  
  global_cpu = (_OLE_cpu_t *)malloc(sizeof(_OLE_cpu_t));     //Allocating CPU
  global_tmp_loop = (_OLE_loop_t *)malloc(sizeof(_OLE_loop_t));  //Allocating first loop instance and keep the root of the LL
  
  global_root_loop = global_tmp_loop;
  
  //Should be 
  global_loopID = loopID;
  
  //
  global_tmp_loop->loopID          = loopID;
  global_tmp_loop->ll_instructions = NULL;
  global_tmp_loop->iter            = 0;
  
  //Building the cache model using the provied configuration
  global_cpu->nb_cores  = config->nb_cores;
  global_cpu->nb_caches = config->nb_caches;
  
  global_cpu->lst_cores = (_OLE_core_t *)malloc(sizeof(_OLE_core_t) * global_cpu->nb_cores);
  
  for (uint64_t i = 0; i < global_cpu->nb_cores; i++)
    {
      global_cpu->lst_cores[i].coreID = i;
      global_cpu->lst_cores[i].lst_caches = (_OLE_cache_t *)malloc(sizeof(_OLE_cache_t) * global_cpu->nb_caches);
      	     
      for (uint64_t j = 0; j < global_cpu->nb_caches; j++)
	{
	  
	  global_cpu->lst_cores[i].lst_caches[j].level    = config->lst_caches[j].level;
	  global_cpu->lst_cores[i].lst_caches[j].assoc    = config->lst_caches[j].assoc;
	  global_cpu->lst_cores[i].lst_caches[j].size     = config->lst_caches[j].size;
	  global_cpu->lst_cores[i].lst_caches[j].cost     = config->lst_caches[j].cost;
	  global_cpu->lst_cores[i].lst_caches[j].linesize = config->lst_caches[j].linesize;
	  global_cpu->lst_cores[i].lst_caches[j].nb_sets  = config->lst_caches[j].nb_sets;
	  global_cpu->lst_cores[i].lst_caches[j].rep_pol  = config->lst_caches[j].rep_pol;
	  
	  global_cpu->lst_cores[i].lst_caches[j].linesin = 0; //This is a counter, must be initialized.
	  global_cpu->lst_cores[i].lst_caches[j].linesout = 0; //This is a counter, must be initialized.
	  
	  global_cpu->lst_cores[i].lst_caches[j].LEN_TAG  = config->lst_caches[j].LEN_TAG;
	  global_cpu->lst_cores[i].lst_caches[j].LEN_SET  = config->lst_caches[j].LEN_SET;
	  global_cpu->lst_cores[i].lst_caches[j].LEN_OFF  = config->lst_caches[j].LEN_OFF;
	  
	  OLE_set_rep_pol_fct_ptr(&global_cpu->lst_cores[i].lst_caches[j],  
				  global_cpu->lst_cores[i].lst_caches[j].rep_pol, 
				  global_cpu->lst_cores[i].lst_caches[j].assoc);
	  
	  global_cpu->lst_cores[i].lst_caches[j].lst_sets = (_OLE_set_t *)malloc(sizeof(_OLE_set_t) * config->lst_caches[j].nb_sets);
	  
	  for (uint64_t k = 0; k < config->lst_caches[j].nb_sets; k++)
	    {
	      global_cpu->lst_cores[i].lst_caches[j].lst_sets[k].linesin = 0;
	      
	      global_cpu->lst_cores[i].lst_caches[j].lst_sets[k].lst_cachelines = 
		(_OLE_cacheline_t *)malloc(sizeof(_OLE_cacheline_t) * config->lst_caches[j].assoc);  
	      
	      //Set cachelines as free !
	      for (uint64_t ii = 0; ii < global_cpu->lst_cores[i].lst_caches[j].assoc; ii++)
		{
		  global_cpu->lst_cores[i].lst_caches[j].lst_sets[k].lst_cachelines[ii].state = 'f';
		  global_cpu->lst_cores[i].lst_caches[j].lst_sets[k].lst_cachelines[ii].owner_addr = 0;
		  global_cpu->lst_cores[i].lst_caches[j].lst_sets[k].lst_cachelines[ii].owner_instance = -1;
		  
		}
	      
	      //Define replacement policy
	      OLE_set_rep_pol_var(&global_cpu->lst_cores[i].lst_caches[j].lst_sets[k], 
				  global_cpu->lst_cores[i].lst_caches[j].rep_pol, 
				  global_cpu->lst_cores[i].lst_caches[j].assoc);
	    }
	}
    }
  
  return global_cpu;
}

//
void OLE_relase()
{
  //free
}

//
_OLE_cpu_t *OLE_init(char *bin, char *xp, uint64_t loopID) //
{ 
  if (xp[strlen(xp) - 1] == '/')
    xp[strlen(xp) - 1] = 0;
  
  //Setting the binary file name ...
  strcpy(bfname, bin);
  strcpy(xpath, xp);
  sprintf(cfg_path, "%s/cfg/cfg.lua", xpath);
  sprintf(out_path, "%s/out", xpath);
  
  strcpy(_dbg_levels[L1], "L1");
  strcpy(_dbg_levels[L2], "L2");
  strcpy(_dbg_levels[L3], "L3");
  
  strcpy(_dbg_events[HIT], "HIT");
  strcpy(_dbg_events[COLD_MISS], "COLD_MISS");
  strcpy(_dbg_events[CONF_MISS], "CONF_MISS");
  strcpy(_dbg_events[CAPA_MISS], "CAPA_MISS");

  //Dump marker
  dumped = 0;
  
  //Should be parametrized !!!
  global_config = OLE_load_config(cfg_path);
  
  global_cpu = OLE_init_(global_config, loopID);
  
  global_trace = NULL;

  //Should handle strings better!!! 
  global_length = 50;
  
  printf("OLE : Initialization complete ...\n");
  
  return global_cpu;
}

//
void OLE_next_iter()
{
  global_tmp_loop->iter++;
}

//Clean up all memory !
void OLE_loop_record()
{
  //Pack instance data
  //Hash packed data (md5)
  //Look up hash

  //If found then insert InstanceID into the found instance clones list
  //If not found insert new Instance
  
  nb_instances++;
  
  if (nb_instances < MAX_INSTANCES)
    {
      //Allocate new instance
      global_tmp_loop->next = (_OLE_loop_t *)malloc(sizeof(_OLE_loop_t)); // New instance entry ...
      
      //
      global_tmp_loop->next->next            = NULL;
      global_tmp_loop->next->iter            = 0;
      global_tmp_loop->next->ll_instructions = NULL;
      global_tmp_loop->next->nb_cache_lines  = 0;
      global_tmp_loop->next->loopID          = global_tmp_loop->loopID;
      
      global_tmp_loop = global_tmp_loop->next;
    }
  else
    if (nb_instances >= MAX_INSTANCES)
      if (!dumped) //Dump once when MAX instances is reached
	{
	  OLE_dump_global_loop_stats();
	  dumped = 1;
	}
  
  return ; 
}

//Depth analysis (stupid but useful!)
void OLE_analyze_pattern(_OLE_instruction_t *inst)
{
  uint64_t diff = 0;
  
  for (uint64_t i = 1, j = 0; i < inst->nb_addresses; i++, j++)
    inst->deltas[j] = DIFF(inst->addresses[i], inst->addresses[i - 1]);
  
  for (uint64_t i = 1; i < inst->nb_addresses - 1 && !diff; i++)
    diff = ((inst->deltas[i - 1]) ^ (inst->deltas[i])); //0 ^ 0

  if (!diff)
    {
      inst->pattern_type = 'l';
      inst->stride = inst->deltas[0];
    }
  else
    {
      inst->pattern_type = 'n';
      inst->stride = 0;
    }
}

//
void OLE_exit()
{
  if (!dumped)
    {
      atexit(OLE_dump_global_loop_stats);
      dumped = 1;
    }
}
