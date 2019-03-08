--
module ("postproc", package.seeall)

--
function ole_read_meta(xp)
   
   local meta_tab = {}
   local f = io.open(xp.."meta/OLE_analyze.meta", "r")

   local line1 = f:read("*line")
   local line2 = f:read("*line")

   local line3 = f:read("*line") -- Skip
   
   local line4 = f:read("*line") 

   local tmp1 = String:get_tokenized_table(line1, ":", false, false, nil)
   local tmp2 = String:get_tokenized_table(line2, ":", false, false, nil)
   local tmp4 = String:get_tokenized_table(line4, ":", false, false, nil)
   
   local binary = tmp1[2]:gsub(" ", "")
   local loopid = tmp2[2]:gsub(" ", "")
   local insncount = tonumber(tmp4[2])

   f:close()
   
   return binary, loopid, insncount
end

-- Generate an output with the complete loop assembly code 
function ole:ole_postproc_csv(xp)

   local asmtab = {}
   local binary, loopid, insncount = ole_read_meta(xp)
 
   local asmfile = xp.."meta/OLE_loop_"..loopid..".asm"
   local csvfile = xp.."out/OLE_"..binary.."_stats.csv"
   local outfile = xp.."out/_OLE_"..binary.."_stats.csv"
    
   local asmf = io.open(asmfile, "rb")
   if (asmf == nil) then print("Cannot find assembly file"); os.exit(-1); end

   local csvf = io.open(csvfile, "rb")
   if (csvf == nil) then print("Cannot find CSV file"); os.exit(-1); end

   local outf = io.open(outfile, "wb")
   if (outf == nil) then print("Cannot find output file"); os.exit(-1); end
   
   for asmline in asmf:lines()
   do
      local tmp = String:get_tokenized_table(asmline, ';', false, false, nil)
      
      table.insert(asmtab, { address = tmp[1], insnline = asmline })
   end
   
   for csvline in csvf:lines()
   do
      header0 = csvline
      junk = csvf:read("*line")
      
      header1 = csvf:read("*line")
      junk = csvf:read("*line")

      insnline = csvf:read("*line")
      
      local csvtab = {}
      
      while (insnline ~= "")
      do
	 tmp = String:get_tokenized_table(insnline, ';', false, false, nil)
	 
	 table.insert(csvtab, { address = tmp[1], insnline = insnline})
    
  	 insnline = csvf:read("*line")
      end
      
      sums = csvf:read("*line")
      ratios = csvf:read("*line")
      junk = csvf:read("*line")
      
      -- Dump header0 & header1
      outf:write(header0.."\n")
      outf:write("\n")
      outf:write(header1.."\n")
      outf:write("\n")
      
      -- Dump asm instructions. If found dump CSV if not dump ASM
      for _, ainsn in pairs(asmtab)
      do
	 insn_str = ainsn.insnline
	 
	 for _, cinsn in pairs(csvtab)
	 do
	    if (ainsn.address == cinsn.address)
	    then
	       insn_str = cinsn.insnline
	    end
	 end
	 
	 outf:write(insn_str..";\n")
      end
      
      outf:write("\n")
      outf:write(sums.."\n")
      outf:write(ratios.."\n")
      outf:write("\n")
      
   end
   
   asmf:close()
   csvf:close()
   outf:close()
end

-- HTML
function ole:ole_stats_to_html(xp, d)
   
   print("Converting CSV to HTML ...");
   
   local binary, loopid, insncount = ole_read_meta(xp)

   -- Remove the jump & the compare
   insncount = insncount - 2
   
   local metafile = xp.."meta/OLE_analyze.meta"
   local asmfile = xp.."meta/OLE_loop_"..loopid..".asm"
   local csvfile = xp.."out/_OLE_"..binary.."_stats.csv"

   -- THESE SHOULD BE PARAMETERS
   local srcfile1 = xp.."../codelet.c"    
   local srcfile2 = xp.."../codelet.f90"
   local datfile  = xp.."../codelet.data"
   local drvfile  = xp.."../driver.f90"

   local outfile = xp.."out/index.html"
   
   local metaf = io.open(metafile, "rb")
   local asmf = io.open(asmfile, "rb")
   local csvf = io.open(csvfile, "rb")
   
   local srcf = io.open(srcfile1, "rb")
   local datf = nil -- io.open(datfile, "rb")
   local drvf = nil -- io.open(drvfile, "rb")

   local outf = io.open(outfile, "wb")
   local header0, header1 = "", ""

   local blocksize = 16
   
   if (srcf == nil)
   then 
      srcf = io.open(srcfile2, "rb")	 
   end
   
   outf:write("<html>\n")
   outf:write("<head><title>"..xp.."</title>")
   
   outf:write([[<style>
   table {
   font-family: lucida;
   font-size: 13;
   border-collapse: collapse;
   width: 80%;
   }

   td, th {
   border: 1px solid #e9f3fc;
   text-align: left;
   padding: 8px;
   }

   tr:nth-child(even) {
    background-color: #e9f3fc;
   }
   </style>
   ]])
 
   outf:write("</head>\n")
   outf:write("<body>\n")
   
   -- outf:write("<center><img src='pix/OLE.png' align='center'>\n")
   
   outf:write("<center><h2><p style='border:3px; border-style:solid; border-color:black; background-color:#16609c; padding: 1;'><font color=white>OLE memory analysis report</font></h2></center>")
   
   if (metaf ~= nil)
   then 
      -- Insert OLE meta analysis (parse lines and tabelize)
      outf:write("<center><u><h2 style='color:white; background-color:#16609c'> Static Analysis</h2></u></center>\n")
   
      outf:write("<center><table>\n")
      for metaline in metaf:lines()
      do
	 --
	 outf:write("<tr bgcolor='#16609c'>\n")
	 
	 --
	 local tmp = String:get_tokenized_table(metaline, ":", false, false, nil)	 

	 --
	 if (tmp[2])
	 then 
	    outf:write("<th bgcolor='#16609c'><font color='black'><center>"..tmp[1].."</center></font></th>\n")
	    outf:write("<th bgcolor='#e9f3fc'><font color='black'>"..tmp[2].."</font></th>\n")
	 else
	    outf:write("<th><font color='black'></font></th>\n")
	    outf:write("<th bgcolor='#e9f3fc'><font color='black'>"..tmp[1].."</font></th>\n")
	 end

	 outf:write("</tr>\n")
      end

      outf:write("</table></center>")
   end
   
   outf:write("<br><br>")
      
   if (asmf ~= nil)
   then 
      -- Insert assembly code (tabelize & color code)
      outf:write("<center><u><h2 style='color: white; background-color:#16609c'>Assembly Code ("..binary..")</h2></u></center>\n")

      outf:write("<center><table>");

      outf:write("<tr><th bgcolor='#16609c'><font color='white'>Address</font></th>")
      outf:write("<th bgcolor='#16609c'><font color='white'>Instruction</font></th></tr>")
      
      for asmline in asmf:lines()
      do
	 outf:write("<tr>\n")
	 
	 local tmp = String:get_tokenized_table(asmline, ";", false, false, nil)
	 
	 outf:write("<th>"..tmp[1].."</th>\n")
	 outf:write("<th>"..tmp[2].."</th>\n")

	 outf:write("</tr>\n");
      end
   end

   outf:write("</table></center>")
   
   outf:write("<br><br>")

   outf:write("<center><u><h2 style='color: white; background-color:#16609c'> Cache Simulation</h2></u></center>\n")
   
   outf:write("<center>\n");
   
   -- Insert OLE dynamic run data
   for csvline in csvf:lines()
   do
      ---------------------
      -- Loop meta-info header
      
      header0 = csvline

      tmp = String:get_tokenized_table(header0, ";", false, false, nil)
      lid = String:get_tokenized_table(tmp[1], ":", false, false, nil)
      iid = String:get_tokenized_table(tmp[2], ":", false, false, nil)
      lpc  = String:get_tokenized_table(tmp[3], ":", false, false, nil)
      nbi = String:get_tokenized_table(tmp[4], ":", false, false, nil)

      outf:write("<table><tr>\n")
      
      outf:write("<th bgcolor='#16609c'><font color='white'>"..lid[1].."</font></th>\n")
      outf:write("<th bgcolor='#16609c'><font color='white'>"..iid[1].."</font></th>\n")
      outf:write("<th bgcolor='#16609c'><font color='white'>"..lpc[1].."</font></th>\n")
      outf:write("<th bgcolor='#16609c'><font color='white'>Nb load/store</font></th>\n")
      outf:write("<th bgcolor='#16609c'><font color='white'>"..nbi[1].."</font></th>\n")

      outf:write("</tr><tr>\n")

      outf:write("<td>"..lid[2].."</td>")
      outf:write("<td>"..iid[2].."</td>")
      outf:write("<td>"..lpc[2].."</td>")
      outf:write("<td>"..nbi[2].."</td>")
      outf:write("<td>"..insncount.."</td>")

      outf:write("</tr>")
    
      outf:write("</table>")
      
      junk = csvf:read("*line")
      
      ---------------------
      -- Data table header
      
      outf:write("<table><tr bgcolor='#16609c'>\n")

      header1 = csvf:read("*line")
      tmp = String:get_tokenized_table(header1, ";", false, false, nil)
      
      for _, v in pairs(tmp)
      do
	 if (v ~= "" and v ~= " " and _ ~= 4)
	 then 
	    outf:write("<th bgcolor='#16609c'><font color='white'>"..v.."</font></th>\n")
	 end
      end
      
      outf:write("</tr>\n")

      junk = csvf:read("*line")

      ---------------------
      -- Simulation data

      local insnline = " "
      local linecount = 0
      
      while (insnline ~= "")
      do
	 outf:write("<tr bgcolor='#e0ffff'>\n")

	 tmp = String:get_tokenized_table(insnline, ';', false, false, nil)
	 
	 for _, v in pairs(tmp)
	 do	    
	    if (v ~= "" and v ~= " " and _ ~= 4)
	    then 
	       outf:write("<td><b>"..v.."</b></td>\n")
	    end
	 end
	 
	 outf:write("</tr>\n")

	 -- Insert blank row when block
	 if (linecount ~= 0 and (linecount % blocksize) == 0)
	 then
	    outf:write("<tr class='blank_row'><td colspan='14'><hr> </td></tr>\n")
	 end
	 
	 linecount = linecount + 1
  	 insnline = csvf:read("*line")
      end

      outf:write("<tr class='blank_row'><td colspan='14'><hr> </td></tr>\n")

      -- Sums      
      sums = csvf:read("*line")

      tmp  = String:get_tokenized_table(sums, ":", false, false, nil)
      tmp1 = String:get_tokenized_table(tmp[2], ";", false, false, nil)

      outf:write("<tr><th bgcolor='#6082b6' colspan='4'><font color='white'>"..tmp[1].."</font></th>\n")
      
      for _, v in pairs(tmp1)
      do
	 outf:write("<td bgcolor='#6082b6'><b>"..v.."</b></td>\n")
      end
      
      outf:write("</tr>\n")
      
      -- Ratios
      ratios = csvf:read("*line")

      tmp  = String:get_tokenized_table(ratios, ":", false, false, nil)
      tmp1 = String:get_tokenized_table(tmp[2], ";", false, false, nil)

      outf:write("<tr><th bgcolor='#6082b6' colspan='4'><font color='white'>"..tmp[1].."</font></th>\n")
      
      for _, v in pairs(tmp1)
      do
	 outf:write("<td bgcolor='#6082b6'><b>"..v.."</b></td>\n")
      end
     
      outf:write("</tr></table>\n")

      outf:write("<br><br>\n")

      junk = csvf:read("*line")
   end
   
   outf:write("</center>\n");
   
   outf:write("</body>\n")
   outf:write("</html>\n")
   
   outf:close()
   csvf:close()
   asmf:close()
   metaf:close()

   if (srcf ~= nil)
   then
      srcf:close()
   end

   if (drvf ~= nil)
   then
      drvf:close()
   end

   if (datf ~= nil)
   then
      datf:close()
   end
   
end
