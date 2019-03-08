module ("metaooo", package.seeall)

--
local function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- 
function ole_get_OLE_trace_tab(trace_fname, dup)
   
   local i = 1
   local trace_tab = {}
   local cacheline_tab = {}
   local cacheline_count = 0
   local cacheline_share_count = 0
   local f = io.open(trace_fname, "rb")
  
   while (i <= tonumber(dup))
   do
      local j = 1
      local trace_tab_entry = {}
      local iter_line = f:read("*line")
      local tmp_iter = String:get_tokenized_table(iter_line, "-", false, false, nil)

      tmp_iter[2] = trim(tmp_iter[2]):gsub(":", "")
      
      cacheline_share_count = 0

      while (j <= tonumber(tmp_iter[2]))
      do
	 local line = f:read("*line")
	 local tmp_line = String:get_tokenized_table(line, ",", false, false, nil)
	 
	 tmp_line[1] = trim(tmp_line[1]):gsub("%(", "")
	 tmp_line[2] = trim(tmp_line[2])
	 tmp_line[3] = trim(tmp_line[3])
	 tmp_line[4] = trim(tmp_line[4]) -- Cache line
	 tmp_line[5] = trim(tmp_line[5]):gsub("%);", "")
	 
	 if (cacheline_tab[tmp_line[4]] == nil)
	 then
	    cacheline_tab[tmp_line[4]] = cacheline_count
	    cacheline_count = cacheline_count + 1
	 else
	    cacheline_share_count = cacheline_share_count + 1
	 end

	 print (tmp_line[1].." "..tmp_line[4].." "..cacheline_tab[tmp_line[4]])	  
	 
	 trace_tab_entry[trim(tmp_line[1])] = { cache_level = tmp_line[3], 
						cache_line = trim(tmp_line[4]),
						cache_lineID = cacheline_tab[tmp_line[4]]}
	 
	 j = j + 1
      end
      
      print ("---- Share count: "..cacheline_share_count)
      
      trace_tab[i] = trace_tab_entry
      
      f:read("*line")
      
      i = i + 1
   end
      
   return trace_tab
end

function ole:ole_generate_decorated_OoO_file(ooo_out_fname, ooo_fname, trace_fname, dup)

   local i = 1
   local ooo_out_f = io.open(ooo_out_fname, "wb")
   local trace_tab = ole_get_OLE_trace_tab(trace_fname, dup)
   
   local ooo_header = "instruction;                                            nb_fe;  type;           inputs;         outputs;        latency;        P0,P1,P2,P3,P4,P5,P6,P7;        special;        type;           inputs;         outputs;        latency;        P0,P1,P2,P3,P4,P5,P6,P7;        special;"   
   
   ooo_out_f:write(ooo_header.."\n")
   
   while (i <= tonumber(dup))
   do
      local ooo_f = io.open(ooo_fname, "rb")
      
      ooo_f:read("*line")
      
      for line in ooo_f:lines()
      do
	 local index
	 local tmp = String:get_tokenized_table(line, ";", false, false, nil)
	 local tmp_insn = String:get_tokenized_table(tmp[1], ":", false, false, nil)


	 -- Use a switch to handle both DECIMAL & HEX address formats
	 local address = "0x"..string.format("%x", tonumber(tmp_insn[1])) 
	 
	 local trace_tab_entry = trace_tab[i]
	 
	 if (trace_tab_entry[address])
	 then
	    if (trim(tmp[3]) == "load" or trim(tmp[3]) == "store")
	    then
	       -- Update special field
	       tmp[ 8] = tmp[ 8].."cache_level:"..trace_tab[i][address].cache_level.." ,cache_line:"..trace_tab[i][address].cache_lineID
	    elseif (trim(tmp[9]) == "load" or trim(tmp[9]) == "store")
	    then
	       -- Update other special field
	       tmp[14] = tmp[14].."cache_level:"..trace_tab[i][address].cache_level.." ,cache_line:"..trace_tab[i][address].cache_lineID
	    end
	 end
	 
	 for k = 1, #tmp - 1
	 do
	    ooo_out_f:write(tmp[k]..";")
	 end
    
	 ooo_out_f:write("\n")
      
      end
      
      ooo_f:close()
      
      i = i + 1
   end

   ooo_out_f:close()
end

