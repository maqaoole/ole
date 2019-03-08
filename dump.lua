--
module ("dump", package.seeall)

--
function ole:ole_dump_loops_meta(loops_meta, binary, xp)

   local f
   
   for klm, loop_meta in pairs(loops_meta)
   do    
      f = io.open(xp.."meta/OLE_analyze.meta", "w")
            
      f:write("Binary             : "..binary.."\n")
      f:write("Loop ID            : "..loop_meta.loop_ptr:get_id().."\n")
      f:write("Origin function    : "..loop_meta.func_ptr:get_name().."\n")
      f:write("Nb of Instructions : "..loop_meta.loop_ptr:get_ninsns().."\n")
      f:write("Bytes per iter     : "..loop_meta.memory_per_iter.."\n")
      f:write("Avg cycles estim.  : "..((loop_meta.loop_cycles["min"] + loop_meta.loop_cycles["max"]) / 2).."\n")
      f:write("BW (Bytes/cycle)   : "..loop_meta.memory_per_iter / 
		 ((loop_meta.loop_cycles["min"] + loop_meta.loop_cycles["max"]) / 2).."\n")
      
      for ks, stream in pairs(loop_meta.streams)
      do
	 f:write ("Stream #"..ks..":\n")
	 for ksa, stream_address in pairs(stream.stream_addresses)
	 do
	    f:write(ksa.." "..stream_address.insn_str.."\n")
	 end
      end
      
      f:close()
   end
end

--
function ole:ole_dump_insns_meta(insns_meta, binary, xp)
   
   local f
   
   for klm, loop_meta in pairs(insns_meta)
   do    
      f = io.open(xp.."meta/OLE_loop_"..loop_meta.loop_ptr:get_id()..".meta", "w")
            
      f:write("Loop ID           : "..loop_meta.loop_ptr:get_id().."\n")
      f:write("Origin function   : "..loop_meta.func_ptr:get_name().."\n")
      f:write("Bytes per iter    : "..loop_meta.memory_per_iter.."\n")
      f:write("Avg cycles estim. : "..((loop_meta.loop_cycles["min"] + loop_meta.loop_cycles["max"]) / 2).."\n")
      f:write("BW (Bytes/cycle)  : "..loop_meta.memory_per_iter / 
		 ((loop_meta.loop_cycles["min"] + loop_meta.loop_cycles["max"]) / 2).."\n")
      
      for addr, insn in pairs(loop_meta.insns)
      do
	 f:write(addr.." "..(insn:tostring():gsub("Insn: ", "")):gsub("\t", " ").."\n")
      end
      
      f:close()
   end
   
end

-- Merges OLE data and groups into an xlsx spreadsheet
function ole:ole_dump_xls(data_file, groups_file)


end

