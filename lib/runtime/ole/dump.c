#include "init.h"
#include "dump.h"
#include "inst.h"
#include "bitwz.h"
#include "consts.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

//
int OLE_dump_trace(_OLE_trace_t *root, _OLE_loop_t *loop, char *ofname)
{
  _OLE_trace_meta_t *m_tmp;
  _OLE_trace_t *t_tmp = root;
  FILE *fd = fopen(ofname, "wb");
  uint64_t nb_inst = OLE_get_nb_instructions(loop);
  
  //Dumping the trace ...
  while (t_tmp)
    {
      fprintf(fd, "0x%" PRIx64 ":\n  ", t_tmp->addr);
      
      m_tmp = t_tmp->meta;
      
      //Dumping meta info ... 
      while (m_tmp)
	fprintf(fd, " (0x%" PRIx64 ", %" PRIu64 ")", m_tmp->bin_addr, m_tmp->iter / nb_inst), m_tmp = m_tmp->next;
      
      fprintf(fd, "\n");
      
      t_tmp = t_tmp->next;
    }
  
  fclose(fd);
  
  return 0;
}

//
void OLE_dump_groups(FILE *fd, _OLE_loop_t *loop)
{  
  for (uint64_t i = 0; i < loop->nb_groups && i < MAX_ITER; i++)
    {
      fprintf(fd, "ITER %" PRIu64 " - %" PRIu64 ":\n", i, 
	      loop->lst_groups[i].nb_instructions);
      
      for (uint64_t j = 0; j < loop->lst_groups[i].nb_instructions && j < MAX_INSN; j++)
	{
	  fprintf(fd, "\t(0x%" PRIx64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", 0x%016" PRIx64 ", %" PRIu64 ");\n", 
		  loop->lst_groups[i].lst_instructions[j].bin_addr, 
		  loop->lst_groups[i].lst_instructions[j].iter,
		  loop->lst_groups[i].lst_instructions[j].memlev,
		  loop->lst_groups[i].lst_instructions[j].cacheline_address, //Base 10
		  loop->lst_groups[i].lst_instructions[j].cacheline_address, //Base 16
		  loop->lst_groups[i].lst_instructions[j].cacheline_offset);
	}
      
      fprintf(fd, "\n");
    } 
}


//Dumps the stats of each instruction in a loop into 
void OLE_dump_loop_all_stats(FILE *fd, _OLE_loop_t *loop_p)
{
  _OLE_loop_t *loop = loop_p;
  _OLE_instruction_t *inst;
  uint64_t cnt_instances = 0, nb_insns = 0, nb_iter;
  uint64_t stats_per_col[NB_COLS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  while (loop)
    {            
      inst = loop->ll_instructions;
      
      if (cnt_instances < nb_instances)
	{
	  nb_insns = OLE_get_nb_instructions(loop);

	  nb_iter  = loop->iter; //nb_calls / (nb_instances * nb_insns);
	  
	  //Printing loopID and effective number of iterations
	  fprintf(fd, "LoopID : %" PRIu64 "; InstanceID : %" PRIu64 "; LoopCount : %" PRIu64 "; NbInsns : %" PRIu64 "\n\n",
	  	  loop->loopID,
	  	  cnt_instances,
	  	  nb_iter, 
		  nb_insns);
	  
	  cnt_instances++;
	  
	  //Zero up for next round    
	  stats_per_col[L1_HIT]  = 0;   //L1 hits 
	  stats_per_col[L1_COLD] = 0;   //L1 cold misses 
	  stats_per_col[L1_CONF] = 0;   //L1 conflict misses
	  
	  stats_per_col[L2_HIT]  = 0;   //L2 hits 
	  stats_per_col[L2_COLD] = 0;   //L2 cold misses 
	  stats_per_col[L2_CONF] = 0;   //L2 conflict misses

	  stats_per_col[L3_HIT]  = 0;   //L3 hits 
	  stats_per_col[L3_COLD] = 0;   //L3 cold misses 
	  stats_per_col[L3_CONF] = 0;   //L3 conflict misses
	  	  
	  //Printing stats per instruction (CSV)
	  //Walk through the instructions LL & the caches LL and print hits ! 
	  fprintf(fd, "Address; Instruction                                        ;  Type; "
		      "Leader/Follower; Leader address; "
		      "L1_HIT; L1_COLD; L1_CONF; "
		      "L2_HIT; L2_COLD; L2_CONF; "
		      "L3_HIT; L3_COLD; L3_CONF;\n\n");
	  	  
	  while (inst)
	    {	      
	      fprintf(fd, "0x%" PRIx64 "; %*s;\t%s;\t%c;\t0x%" PRIx64 "%s\t"
		      "%" PRIu64 ";\t%" PRIu64 ";\t%" PRIu64 ";\t"
		      "%" PRIu64 ";\t%" PRIu64 ";\t%" PRIu64 ";\t"
		      "%" PRIu64 ";\t%" PRIu64 ";\t%" PRIu64 "\n",
		      inst->bin_address,
		      -global_length, 
		      (inst->asmline) ? inst->asmline : "NULL", 
		      (inst->type == 'l') ? "load" : "store",
		      (inst->bin_address == inst->laddr) ? 'l' : 'f',
		      (inst->laddr),
		      (inst->laddr) ? ";" : ";\t",
		      
		      inst->stats.lst_stats[L1].hit,        //L1 hits 
		      inst->stats.lst_stats[L1].cold_miss,  //L1 cold misses 
		      inst->stats.lst_stats[L1].conf_miss,  //L1 conflict misses
		      
		      inst->stats.lst_stats[L2].hit,        //L2 hits 
		      inst->stats.lst_stats[L2].cold_miss,  //L2 cold misses 
		      inst->stats.lst_stats[L2].conf_miss,  //L2 conflist misses
		      
		      inst->stats.lst_stats[L3].hit,        //L3 hits
		      inst->stats.lst_stats[L3].cold_miss,  //L3 cold misses
		      inst->stats.lst_stats[L3].conf_miss); //L3 conflict misses
	      
	      stats_per_col[L1_HIT] += inst->stats.lst_stats[L1].hit;          //L1 hits 
	      stats_per_col[L1_COLD] += inst->stats.lst_stats[L1].cold_miss;   //L1 cold misses 
	      stats_per_col[L1_CONF] += inst->stats.lst_stats[L1].conf_miss;   //L1 conflict misses

	      stats_per_col[L2_HIT] += inst->stats.lst_stats[L2].hit;          //L2 hits 
	      stats_per_col[L2_COLD] += inst->stats.lst_stats[L2].cold_miss;   //L2 cold misses 
	      stats_per_col[L2_CONF] += inst->stats.lst_stats[L2].conf_miss;   //L2 conflict misses

	      stats_per_col[L3_HIT] += inst->stats.lst_stats[L3].hit;          //L3 hits 
	      stats_per_col[L3_COLD] += inst->stats.lst_stats[L3].cold_miss;   //L3 cold misses 
	      stats_per_col[L3_CONF] += inst->stats.lst_stats[L3].conf_miss;   //L3 conflict misses
	    
	      inst = inst->next;
	    }
	  
	  fprintf(fd, 
		  "\nSum(Column):\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf;\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf;\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf\n",
		  (double)stats_per_col[L1_HIT], (double)stats_per_col[L1_COLD], (double)stats_per_col[L1_CONF], 
		  (double)stats_per_col[L2_HIT], (double)stats_per_col[L2_COLD], (double)stats_per_col[L2_CONF], 
		  (double)stats_per_col[L3_HIT], (double)stats_per_col[L3_COLD], (double)stats_per_col[L3_CONF]);
	  
	  fprintf(fd, 
		  "Sum(Column) / LoopCount:\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf;\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf;\t"
		  "%0.3lf;\t%0.3lf;\t%0.3lf\n\n",
		  (double)stats_per_col[L1_HIT] / nb_insns, (double)stats_per_col[L1_COLD] / nb_insns, (double)stats_per_col[L1_CONF] / nb_insns, 
		  (double)stats_per_col[L2_HIT] / nb_insns, (double)stats_per_col[L2_COLD] / nb_insns, (double)stats_per_col[L2_CONF] / nb_insns, 
		  (double)stats_per_col[L3_HIT] / nb_insns, (double)stats_per_col[L3_COLD] / nb_insns, (double)stats_per_col[L3_CONF] / nb_insns);
	}
      else
	loop = NULL;

      if (loop)
	loop = loop->next;
    }
  
  return;
}

//
void OLE_dump_paxt(FILE *fd, _OLE_loop_t *loop)
{
  uint64_t nb_instances = 0;
  _OLE_instruction_t *inst = NULL;
  
  while (loop)
    {
      inst = loop->ll_instructions;
      
      fprintf(fd, "Instance %lld\n", nb_instances++);
      
      while (inst)
	{
	  OLE_analyze_pattern(inst);
	  
	  fprintf(fd, "0x%" PRIx64 ":\t%*s;\t",
		  inst->bin_address,
		  -global_length, 
		  (inst->asmline) ? inst->asmline : "NULL");

	  if (inst->pattern_type == 'l')
	    {
	      fprintf(fd, "linear pattern;\t");

	      if (inst->stride)
		fprintf(fd, "%lld\n", inst->stride);
	      else
		fprintf(fd, "constant\n");
	    }
	  else
	    {
	      fprintf(fd, "non linear pattern:");
	      
	      for (int i = 0; i < inst->nb_addresses - 1; i++)
		fprintf(fd, " %lld", inst->deltas[i]);
	      
	      fprintf(fd, "\n");
	    }
	  
	  inst = inst->next;
	}
      
      fprintf(fd, "\n");

      loop = loop->next;
    }
}

//
void OLE_dump_4K_events(FILE *fd)
{
  for (uint64_t i = 0; i < nb_4K_alias_events; i++)
    {
      fprintf(fd, 
	      "Load  insn @   : 0x%" PRIx64 "\n"
	      "Load  memory @ : 0x%" PRIx64 "\n"
	      "Store insn @   : 0x%" PRIx64 "\n"
	      "Store memory @ : 0x%" PRIx64 "\n\n",
	      alias_event_tab[i].load_baddr,
	      alias_event_tab[i].load_iaddr,
	      alias_event_tab[i].store_baddr,
	      alias_event_tab[i].store_iaddr);
    }
}

//
void OLE_dump_global_loop_stats()
{
  //pid_t tid = syscall(SYS_gettid);
  char fname_all[MAX_STR], fname_paxt[MAX_STR],
       fname_new_grps[MAX_STR], fname_new_grps_cold[MAX_STR],
       fname_new_grps_last[MAX_STR], fname_4K_events[MAX_STR];

  sprintf(fname_all, "%s/OLE_%s_stats.csv", out_path, bfname);
  sprintf(fname_new_grps, "%s/OLE_%s_groups.csv", out_path, bfname);
  sprintf(fname_new_grps_cold, "%s/OLE_%s_groups_cold.csv", out_path, bfname);
  sprintf(fname_new_grps_last, "%s/OLE_%s_groups_last.csv", out_path, bfname);
  
  //sprintf(fname_4K_events, "%s/OLE_4K_aliasing_event.csv", out_path);
  
  sprintf(fname_paxt, "%s/OLE_paxt.csv", out_path);

  FILE *fd = fopen(fname_all, "wb"),
    *fng   = fopen(fname_new_grps, "wb"),
    *fngc  = fopen(fname_new_grps_cold, "wb"),
    *fngl  = fopen(fname_new_grps_last, "wb"),
    *fpaxt = fopen(fname_paxt, "wb");
      
  OLE_dump_loop_all_stats(fd, global_root_loop);    
  fclose(fd);

  OLE_dump_groups(fngc, global_root_loop);
  fclose(fngc);

  OLE_dump_groups(fng, global_root_loop->next);
  fclose(fng);
 
  uint64_t i = 0;
  _OLE_loop_t *tmp = global_root_loop;
  
  while (tmp->next && i < MAX_INSTANCES)
    { tmp = tmp->next; i++; }
  
  printf("OLE instance chosen : %" PRIu64 "/%" PRIu64 "\n", i, nb_instances);

  OLE_dump_groups(fngl, tmp);
  fclose(fngl);

  OLE_dump_paxt(fpaxt, global_root_loop);
  fclose(fpaxt);

  printf("OLE stats dumped in : %s/\n", out_path);
  
  exit(0); //Make sure to quit after !!
  
  return;
}

//Dumps config vertically
void OLE_dump_config_V(FILE *fd, _OLE_config_t *config)
{  
  for (uint64_t i = 0; i < config->nb_caches; i++)
    fprintf(fd, "                Level : %" PRIu64 "\n"
	    "                Size  : %" PRIu64 "\n"
	    "              Nb ways : %" PRIu64 "\n"
	    "                 Cost : %" PRIu64 "\n"
	    "            Line size : %" PRIu64 "\n"
	    "              Nb sets : %" PRIu64 "\n"
	    "   Replacement policy : %s\n"
	    "           TAG length : %" PRIu64 "\n"
	    "           SET length : %" PRIu64 "\n"
	    "        OFFSET length : %" PRIu64 "\n\n",
	    config->lst_caches[i].level,
	    config->lst_caches[i].size,
	    config->lst_caches[i].assoc,
	    config->lst_caches[i].cost,
	    config->lst_caches[i].linesize,
	    config->lst_caches[i].nb_sets,
	    hex2str(config->lst_caches[i].rep_pol),
	    config->lst_caches[i].LEN_TAG,
	    config->lst_caches[i].LEN_SET,
	    config->lst_caches[i].LEN_OFF);
}

//Dumps config horizontally
void OLE_dump_config_H(FILE *fd, _OLE_config_t *config)
{
  fprintf(fd, "Level;\tSize(B);\tNbWays;\t" 
	      "Cost(Cycles);\tLine size(Bytes);\t"
	      "NBSets;\tRep. policy;\t"
	      "TAG len;\tSET len;\tOFFSET len\n");

  for (uint64_t i = 0; i < config->nb_caches; i++)
    fprintf(fd,"%5" PRIu64 ";\t"
	       "%7" PRIu64 ";\t"
	       "%6" PRIu64 ";\t"
	       "%12" PRIu64 ";\t"
	       "%16" PRIu64 ";\t"
	       "%6" PRIu64 ";\t"
	       "%11s;\t"
	       "%7" PRIu64 ";\t"
	       "%7" PRIu64 ";\t"
	       "%10" PRIu64 "\n",
	    config->lst_caches[i].level,
	    config->lst_caches[i].size,
	    config->lst_caches[i].assoc,
	    config->lst_caches[i].cost,
	    config->lst_caches[i].linesize,
	    config->lst_caches[i].nb_sets,
	    hex2str(config->lst_caches[i].rep_pol),
	    config->lst_caches[i].LEN_TAG,
	    config->lst_caches[i].LEN_SET,
	    config->lst_caches[i].LEN_OFF);
  
  fprintf(fd, "\n");
}

//A tiny hack to avoid duplicating code
void OLE_print_config(_OLE_config_t *config)
{ OLE_dump_config_H(stdout, config); }
