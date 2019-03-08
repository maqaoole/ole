#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

#include "consts.h"

/* MEMORY HIERARCHY STRUCTURES */

//
typedef struct _OLE_4K_alias_event_s {
  
  uint64_t load_baddr;
  uint64_t load_iaddr;
  
  uint64_t store_baddr;
  uint64_t store_iaddr;

} _OLE_4K_alias_event_t;

//
typedef struct _OLE_address_s {
  
  uint64_t addr; //64-bit address
  uint64_t TAG;  //Tag bits/value
  uint64_t SET;  //Set bits/value
  uint64_t OFF;  //Offset bits/value
  
} _OLE_address_t;

//
typedef struct _OLE_stats_s {
  
  uint64_t hit;
  uint64_t cold_miss;
  uint64_t conf_miss;
  
} _OLE_stats_t;

//
typedef struct _OLE_instruction_stats_s {
  
  uint64_t RAM_access;     //Supposed to be the sum of all LLC misses
  _OLE_stats_t *lst_stats; //Stats per cache level
  
} _OLE_instruction_stats_t;

//
typedef struct _OLE_instruction_s {
  
  uint64_t bin_address; //Location in the binary
  uint64_t stream_id;   //Origin memory streamID
  unsigned char type;   //Load or store or prefetch
  
  char *asmline;        //Assembly line (used for output)
  
  uint64_t iter;

  unsigned char lstatus; //Leader follower
  unsigned char fstatus; //Follower
  uint64_t laddr;

  unsigned char paxt_done;
  uint64_t nb_addresses;
  uint64_t addresses[MAX_ADDRESSES];
  int64_t deltas[MAX_ADDRESSES - 1];
  int64_t stride;
  unsigned char pattern_type; 
  
  _OLE_instruction_stats_t stats;   //L1, L2, L3, RAM stats : hit/cold_miss/capacity_miss
  
  struct _OLE_instruction_s *next;
  
} _OLE_instruction_t;

//
typedef struct _OLE_group_insn_s {
  
  uint64_t iter;  
  uint64_t memlev;
  uint64_t bin_addr;
  uint64_t cacheline_address;
  uint64_t cacheline_offset;

} _OLE_group_insn_t;

//
typedef struct _OLE_group_s {
  
  uint64_t id; //Groups ID
  
  uint64_t nb_instructions; 
  _OLE_group_insn_t lst_instructions[MAX_INSN]; //Group instructions
  
} _OLE_group_t;


//
typedef struct _OLE_cacheline_s {
  
  unsigned owner_coreID; //Core owning the cacheline
  unsigned char  state;  //Usage state : free (1), busy (0)
  _OLE_address_t addr;   //Address

  unsigned char has_owner;
  uint64_t owner_addr;   //Instruction owning the cacheline (used for grouping) 
  uint64_t owner_memlev;
  int64_t owner_instance;
  
  uint64_t stream_id;
    
} _OLE_cacheline_t;

//
typedef struct _OLE_set_s {
  
  _OLE_cacheline_t *lst_cachelines; //List of cachelines
  
  uint64_t linesin;                 //Number of busy lines 
  
  void *rep_pol_var;                //Replacement policy variable
  
} _OLE_set_t;

//
typedef struct _OLE_cache_s {

  uint64_t level;        //Cache level : L(1), L(2), ...
  uint64_t assoc;        //Cache set associativity
  uint64_t size;         //Cache size in bytes
  uint64_t cost;         //Cost of access in cycles
 
  uint64_t linesize;     //Cache line size
  uint64_t linesin;      //Number of lines inserted ... To check if cache is full ! 
  uint64_t linesout; 
  
  uint64_t   nb_sets;    //Number of sets in the cache level : cache size / (cacheline size * associativity)
  _OLE_set_t *lst_sets;  //List of sets
  
  unsigned rep_pol;      //Replacement policy coded in ASCII hex
  int (*rep_pol_line_fct_ptr)(void *rep_pol_var);       //Line to be replaced
  void (*rep_pol_fct_ptr)(void *rep_pol_var, int line); //Replacement policy function pointer
  
  uint64_t LEN_TAG;      //address TAG length in bits : log2(linesize)
  uint64_t LEN_SET;      //address SET length in bits : log2(nb_sets) 
  uint64_t LEN_OFF;      //address OFFSET length in bits : len_ADDR - len_TAG - len_SET 
  
} _OLE_cache_t;

//
typedef struct _OLE_core_s {
  
  unsigned     coreID;
  _OLE_cache_t *lst_caches; //List of cores
  
} _OLE_core_t;

//
typedef struct _OLE_cpu_s {
  
  uint64_t     nb_cores;               //Number of cores in the CPU
  uint64_t     nb_caches;              //Number of caches per core
  _OLE_core_t  *lst_cores;             //List of cores
  
} _OLE_cpu_t;

//
typedef struct _OLE_config_s {
  
  uint64_t nb_cores;                         //Number of cores
  uint64_t nb_caches;                        //Number of caches
  _OLE_cache_t lst_caches[MAX_CACHE_LEVELS]; //Cache parameters
  
} _OLE_config_t;

/* TRACE STRUCTURES */

//
typedef struct _OLE_trace_meta_s {
  
  uint64_t bin_addr;              //Instruction binary address
  uint64_t iter;                  //Iteration ...
  struct _OLE_trace_meta_s *next; //Next meta info
  
} _OLE_trace_meta_t;

//
typedef struct _OLE_trace_s {
  
  uint64_t addr;               //Memory address : hashtab key
  _OLE_trace_meta_t *meta;     //Meta info about the trace entry 
  struct _OLE_trace_s *next;   //Next trace entry
  
} _OLE_trace_t;


//
typedef struct _OLE_loop_s {
  
  uint64_t loopID;                      //Loop ID given by MAQAO
  
  uint64_t iter;
  
  uint64_t nb_cache_lines;

  uint64_t nb_groups;
  _OLE_group_t lst_groups[MAX_ITER];
  
  uint64_t mem_accesses;                //Number of instructions x number of iterations 
  _OLE_instruction_t *ll_instructions;  //Root for list of instructions
  
  struct _OLE_loop_s *next;             //For managinf multiple loops
  
} _OLE_loop_t;


#endif
