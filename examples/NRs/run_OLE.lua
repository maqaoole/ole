-- This must always be set to the current date
-- This is how OLE builds the experiment directory's name
local date = os.date("*t") 
local ole_dir = "maqao-ole_"..date.day.."-"..date.month.."-"..date.year

-- Open database
local csv_f = io.open("targets.csv", "rb")

-- Discard first line
csv_f:read("*line")

local i = 0

-- For each entry in the database 
for line in csv_f:lines()
do
   if (1)
   then
      local target_tab = String:get_tokenized_table(line, ",", false, false, nil)
      
      local codelet_binname  = target_tab[4]
      local codelet_loopid   = target_tab[5]
      local codelet_datasize = target_tab[8]
      local codelet_nbiter   = target_tab[13]
      local codelet_nbrep    = target_tab[14]

      -- Create a new codelet.data for each run
      local codelet_data_f = io.open("codelet.data", "w")
      codelet_data_f:write(codelet_nbrep.." "..codelet_datasize)
      codelet_data_f:close()
      
      -- OLE Instru
      print ("Instrumenting: "..codelet_binname.." "..codelet_loopid)
      os.execute("maqaodev ole -instru -b="..codelet_binname.." -l="..codelet_loopid)

      -- OLE Run (verify libole before run)
      print ("Running: "..codelet_binname)
      os.execute(ole_dir.."/bin/"..codelet_binname.."_i_ole")

      -- OLE format output
      print ("Formatting ...")
      os.execute("maqaodev ole -output -xp="..ole_dir)
      os.execute("mv "..ole_dir.." "..ole_dir.."."..codelet_binname)

      -- Extract OoO file using CQA
      os.execute("maqaodev cqa -loop="..codelet_loopid.." ./"..codelet_binname.." uo=\"mode=dump, max_iters="..codelet_nbiter.."\" >> /dev/null")

      -- Decorate OOO file
      os.execute("maqaodev ole -ooo -OoO=dump_"..codelet_loopid..".ooo -t="..ole_dir.."."..codelet_binname.."/out/OLE_"..codelet_binname.."_groups.csv -d=1")

      -- move dump into the right directory.
      os.execute("mv dump_"..codelet_loopid..".ooo "..ole_dir.."."..codelet_binname)
      os.execute("mv dump_"..codelet_loopid.."-U1.ooo "..ole_dir.."."..codelet_binname)

      --
      os.execute("mv codelet.data "..ole_dir.."."..codelet_binname)
   end

   i = i + 1

end
-- Close database
csv_f.close()
