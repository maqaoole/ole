--
module ("instru", package.seeall)

--
local function ole_instrument_insn(insn, insn_type, madras_obj, stream_id)

   local insn_str = (insn:tostring():gsub("Insn: ", "")):gsub("\t", " ") -- Get rid of tabs !

   madras_obj:fctcall_new("OLE_inject_cacheline", "ole.so", insn:get_address(), 0, 0)
   madras_obj:fctcall_addparam_imm(insn:get_loop():get_id())
   madras_obj:fctcall_addparam_imm(insn:get_address())
   madras_obj:fctcall_addparam_imm(insn_type)
   madras_obj:fctcall_addparam_from_gvar(nil, insn_str, "a")

   if (insn_type == 1)
   then
      madras_obj:fctcall_addparam_frominsn(insn:get_operand_src_index(), insn:get_address(), 0)
   elseif (insn_type == 0)
   then
      madras_obj:fctcall_addparam_frominsn(insn:get_operand_dest_index(), insn:get_address(), 0)      
   end

   return madras_obj
end

--
local function ole_instrument_insns(insns_meta, mardas_obj)
   
   for lid, loop_meta in pairs(insns_meta)
   do
      for addr, insn in pairs(loop_meta.insns)
      do
	 if (insn:is_load())
	 then 
	    insn_type = 1
	    
	 else
	    if (insn:is_store())
	    then
	       insn_type = 0
	       
	    end
	 end

	 madras_obj = ole_instrument_insn(insn, insn_type, madras_obj)

      end
   end
   
   return madras_obj
end

--
local function ole_instrument_loops(loops_meta, madras_obj)

   local insn
   
   for lid, loop_meta in pairs(loops_meta)
   do
      loop = loop_meta.loop_ptr
      
      for ks, streams in pairs(loop_meta.streams) 
      do
	 for _, insns_ptr in pairs(streams)
	 do
	    cnt = 0
	    
	    for insn_addr, insn_entry in pairs(insns_ptr)
	    do
	       insn = insn_entry.insn
	       
	       if (insn:is_load())
	       then
		  insn_type = 1
		  madras_obj = ole_instrument_insn(insn, insn_type, madras_obj, ks)
	       else
		  if (insn:is_store())
		  then
		     insn_type = 0
		     madras_obj = ole_instrument_insn(insn, insn_type, madras_obj, ks)
		  end
	       end
	    
	       local str_tmp = ""
	       
	       if (insn_type == 1 or insn_type == 0)
	       then
		  str_tmp = "==> INSTRUMENTED"
	       end
	       
	       print ("LOOP: "..loop:get_id().."  INSN : "..string.format("%x", insn:get_address()).." "..insn_entry.insn_str.."\t"..str_tmp)
	       
	    end
	 end
      end
      
      -- Insert functions at each end of each exit block of the loop
      for _, block in pairs(loop:get_exits())
      do
	 local l_insn = block:get_last_insn()
	 local l_addr = l_insn:get_address()
	 local p_insn = l_insn:get_prev()
	 local p_addr = p_insn:get_address()

	 -- Iteration counter
	 madras_obj:fctcall_new("OLE_next_iter", "ole.so", p_addr, 1, 0)
	 
	 -- Instance handler
	 madras_obj:fctcall_new("OLE_loop_record", "ole.so", l_addr, 1, 0)
	
      end
   end
   
   return madras_obj, loop:get_id()
end

--
function ole:ole_instrument_binary(mode, _meta, asmfile, madras_obj, binary, xp)

   local loopID
   
   -- Instrument target object
   if (mode == "loops")
   then
      madras_obj, loopID = ole_instrument_loops(_meta, madras_obj)
   else
      if (mode == "instructions")
      then
	 -- madras_obj = ole_instrument_insns(_meta, madras_obj)
      else
	 if (mode == "functions")
	 then
	    -- Not handled yet!
	 end
      end
   end
   
   local _init = nil

   for func in asmfile:functions()
   do
      if (func:get_name() == "_init")
      then 
	 _init_func = func 
      end
   end
   
   if (_init_func)
   then 
      --
      local _init_addr = _init_func:get_first_insn():get_address()
      
      -- Insert OLE's initialization function call
      madras_obj:fctcall_new("OLE_init", "ole.so", _init_addr, 0, 0)
      
      madras_obj:fctcall_addparam_from_gvar(nil, binary, "a")
      madras_obj:fctcall_addparam_from_gvar(nil, xp, "a")
      madras_obj:fctcall_addparam_imm(loopID)
      
   else
      ole:ole_print_error("ERR_INIT_NOTFOUND", xp)
   end
   
   -- Commit the instrumented changes
   madras_obj:modifs_commit(xp.."bin/"..binary.."_i_ole")
   madras_obj:terminate()
end
