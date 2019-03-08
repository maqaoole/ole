--
module ("analyze", package.seeall)

--
function ole_dump_loop_asm(asmfile, loopid, xp)
   
   local f = io.open(xp.."meta/OLE_loop_"..loopid..".asm", "wb")
   
   for func in asmfile:functions()
   do
      for loop in func:loops()
      do
	 if (tonumber(loopid) == loop:get_id())
	 then
	    for block in loop:blocks()
	    do
	       for insn in block:instructions()
	       do
		  local insn_str = insn:tostring():gsub("Insn: ", "")
		  
		  f:write("0x"..string.format("%x", insn:get_address()).."; "..insn_str.."\n")
	       end
	    end
	 end
      end
   end
   
   f:close()
end

-- Analyze loops : apply grouping and CQA
function ole:ole_analyze_loops(loops, asmfile, proj, xp)
   
   local loops_ptr = {}
   local loops_meta = {}
   local processed_loops = 0
   local grouping_mode  = Consts.x86_64.SSE_OFF
   local cqa_results, cqa_context, cqa_path      = {}, {}, {}
   
   cqa_context               = cqa.api.data_struct.new ("cqa_context")
   cqa_context.arch          = asmfile:get_arch_name()
   cqa_context.uarch         = proj:get_uarch_name()
   cqa_context.proc 	     = asmfile:get_proc()
   cqa_context.memory_level  = { "L1" }
   cqa_context.ignore_paths  = true
   cqa_context.requested_metrics = {"cycles if clean"}

   -- Gather loop pointers --

   local nb_loops = 0

   -- Go through functions looking for loops
   for f in asmfile:functions()
   do      
      for l in f:loops()
      do	
	 for k, lid in pairs(loops)
	 do
	    if (tonumber(lid) == l:get_id())
	    then	      
	   
	       ole_dump_loop_asm(asmfile, lid, xp)

	       print ("LOOP: "..lid.." ==> ANALYZED")

	       -- Too much analyses take up memory. 
	       f:analyze_groups()
	       f:analyze_groups_extend(grouping_mode)

	       loops_ptr[lid] = { loop = l, func = f }
	       
	       nb_loops = nb_loops + 1
	    end
	 end	
      end
   end
 
   --
   if (nb_loops ~= #loops) 
   then 
      return nil, "ERR_LOOP_NOTFOUND" 
   end
   
   --
   for lid, loop_ptr in pairs(loops_ptr)
   do
      local loop_streams = {}
      local memory_per_iter = 0
      
      if (loop_ptr.loop:has_groups())
      then
	 
	 processed_loops = processed_loops + 1
	 
	 -- A loop can contain multiple groups/streams
	 for group in loop_ptr.loop:groups() 
	 do
	    local stream_addresses = {}
	    local size = group:get_size(grouping_mode)
	   	    
	    if (group:get_memory_status() == "Success")
	    then 
	       memory_per_iter = memory_per_iter + group:get_access_memory()
	    end
	    
	    local i = 0
	    
	    while (i < size)
	    do
	       local key = string.format("0x%x", group:get_insn_n(i, grouping_mode):get_address())
	       stream_addresses[key] = { insn_str = (group:get_insn_n(i, grouping_mode):tostring():gsub("Insn: ", "")):gsub("\t", " "), insn = (group:get_insn_n(i, grouping_mode)) }
	 
	       i = i + 1
	    end
	    
	    table.insert(loop_streams, { stream_addresses = stream_addresses })	    
	 end

	 cqa_results = cqa:get_cqa_results(loop_ptr.loop, cqa_context)
	 
	 for _, path in pairs(cqa_results.paths)
	 do
	    cqa_path = path
	 end
	 
	 table.insert(loops_meta, { loop_ptr = loop_ptr.loop, func_ptr = loop_ptr.func,
				    loop_cycles = cqa_path["cycles if clean"]["L1"],
				    memory_per_iter = memory_per_iter, streams = loop_streams })
      else
	 ole:ole_print_warning("WARN_LOOP_NOGROUPS", lid)
      end	 
   end
   
   return loops_meta, processed_loops
end

-- Analyze inner loops of target function : apply grouping and CQA
function ole:analyze_funcs(funcs, asmfile)
end

-- Locate insns and go back to the loop : apply grouping and CQA
-- Analyze loops : apply grouping and CQA
function ole:ole_analyze_insns(insns, asmfile, proj)

   local insns_ptr = {}
   local loops_ptr = {}
   local insns_meta = {}
   local processed_insns = 0
   local grouping_mode  = Consts.x86_64.SSE_OFF
   local cqa_results, cqa_context, cqa_path      = {}, {}, {}

   cqa_context               = cqa.api.data_struct.new ("cqa_context")
   cqa_context.arch          = asmfile:get_arch_name()
   cqa_context.uarch         = proj:get_uarch_name()
   cqa_context.proc 	     = asmfile:get_proc()
   cqa_context.memory_level  = { "L1" }
   cqa_context.ignore_paths  = true
   cqa_context.requested_metrics = {"cycles if clean"}

   -- Gather loop pointers --
   
   local nb_insns = 0

   -- Go through functions looking for loops
   for f in asmfile:functions()
   do      
      -- Run groups only on targets -- Heavy memory use 
      f:analyze_groups()
      f:analyze_groups_extend(grouping_mode)
      
      for b in f:blocks()
      do
	 for insn in b:instructions()
	 do	       
	    for k, i in ipairs(insns)
	    do
	       i_str = "0x"..i
	       insn_str = string.format("0x%x", insn:get_address()) 
	       
	       if (i_str == insn_str)
	       then
		  if (insn:is_load() or insn:is_store())
		  then
		     	     
		     insns_ptr[i_str] = insn		     
		     nb_insns = nb_insns + 1
		  else
		     return nil, "ERR_INSN_ILLEGAL"
		  end
	       end
	    end
	 end
      end
   end
   
   -- Check if all instruction have been processed
   if (nb_insns ~= #insns) then return nil, "ERR_INSN_NOTFOUND" end

   local loops_tmp = {}
   
   -- Group instruction in loops 
   for addr, insn in pairs(insns_ptr)
   do
      if (insn:get_loop())
      then
	 if (loops_ptr[insn:get_loop():get_id()] == nil)
	 then
	    loops_tmp[insn:get_loop():get_id()] = insn:get_loop()
	    loops_ptr[insn:get_loop():get_id()] = {}
	 end
	 
	 loops_ptr[insn:get_loop():get_id()][addr] = insn
      else
	 --
	 ole:ole_print_warning("WARN_INSN_DISCARDED", addr)
      end
   end

   --
   for lid, insns in pairs(loops_ptr)
   do
      local memory_per_iter = 0 
      
      print("LOOP: "..lid.." ==> ANALYZED")
      
      processed_insns = 0
      
      if (loops_tmp[lid]:has_groups())
      then
	 
	 processed_insns = processed_insns + 1

	 for group in loops_tmp[lid]:groups()
	 do
	    local size = group:get_size(grouping_mode)
	    
	    if (group:get_memory_status() == "Success")
	    then 
	       memory_per_iter = memory_per_iter + group:get_access_memory()
	    end
	    
	 end
      end
      
      cqa_results = cqa:get_cqa_results(loops_tmp[lid], cqa_context)
      
      for _, path in pairs(cqa_results.paths)
      do
	 cqa_path = path
      end
      
      table.insert(insns_meta, { loop_ptr = loops_tmp[lid], func_ptr = loops_tmp[lid]:get_function(),
				 loop_cycles = cqa_path["cycles if clean"]["L1"], insns = loops_ptr[lid],
				 memory_per_iter = memory_per_iter })
      
      for addr, insn in pairs(insns)
      do
	 print("  INSN : "..addr.." "..insn:get_name())
      end
   end
   
   return insns_meta, processed_insns
end
