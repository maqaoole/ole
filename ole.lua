-- OMLET: On the fly Memory Locality Evaluator and Tracer
-- OLE  : On -the-fly Locality Evaluator

module ("ole", package.seeall)

require "lfs"
require "ole.analyze"
require "ole.dump"
require "ole.consts"
require "ole.instru"
require "ole.metaooo"
require "ole.postproc"

-- Toying around !

-- MAQAO banners
local maqao_banner = {

  [[ 
    _     _     _     _     _  
   / \   / \   / \   / \   / \ 
  ( M ) ( A ) ( Q ) ( A ) ( O )
   \_/   \_/   \_/   \_/   \_/ 

  ]],

  [[

  |\  /|    /\     /~~\     /\     /~~\ 
  | \/ |   /__\   |    |   /__\   |    |
  |    |  /    \   \__X   /    \   \__/ 
                                      
  ]],

  [[
                              
  #   #   #   #   #   #    ###  
  ## ##  # #  #   #  # #  #   # 
  # # # ##### #   # ##### #   #  
  #   # #   # ##### #   # #   # 
  #   # #   #     # #   #  ###  
                              
    ]] }

-- OLE banners
local ole_banner = {

   
   [[

                    .---.. ,-..   ,---..  
                   / .-. ) | |    | .-'   
                   | | |(_)| |    | `-.   
                   | | | | | |    | .-'   
                   \ `-' / | `--. |  `--. 
                    )---'  |( __.'/( __.' 
                   (_)     (_)   (__)           

   ]],

   [[

                      )     (                
                   ( /(     )\ )             
                   )\())   (()/(     (       
                  ((_)\     /(_))    )\      
                     ((_)   (_))     ((_)     
                    / _ \   | |      | __|    
                   | (_) |_ | |__  _ | _|  _  
                    \___/(_)|____|(_)|___|(_)    
  
   ]],

   [[

                     OOO     L       EEEE    
                    O   O    L       E       
                    O   O    L       EEE     
                    O   O .. L    .. E    .. 
                     OOO  .. LLLL .. EEEE ..      

   ]] }

-- Meditation --> Better memory usage, an equanimous mind.
local meditate_banner = {

   [[

                           _ooOoo_
                          o8888888o
                          88" . "88
                          (| -_- |)
                           \  =  / 
                       ____/`---'\____
                     .'  \\|     |//  `.
                    /  \\|||  :  |||//  \
                   /  _||||| -:- |||||_  \
                   |   | \\\  -  /'| |   |
                   | \_|  `\`---'//  |_/ |
                   \  .-\__ `-. -'__/-.  /
                 ___`. .'  /--.--\  `. .'___
              ."" '<  `.___\_<|>_/___.' _> \"".
             | | :  `- \`. ;`. _/; .'/ /  .' ; |
             \  \ `-.   \_\_`. _.'_/_/  -' _.' /
   ===========`-.`___`-.__\ \___  /__.-'_.'_.-'================
                           `=--=-'                    
   ]]
}

-- MAQAO command line help
local function ole_usage()

   math.randomseed(os.time())
   
   print (ole_banner[math.random(1, 3)])
   print ("                    Sit tight and RTFM !")
   print (meditate_banner[1])

   print (". OLE instrumentation\n")
   print ("   # maqao ole -instru -b=BINARY -l=MAQAO_LOOP_ID\n")
   
   print (". OLE execution\n")
   print ("   # ./maqao-ole-DATE/bin/BINARY_i_ole PARAMETERS_IF_ANY\n")

   print (". OLE data formatting (output is in maqao-ole-DATE/out)\n")
   print ("  # maqao ole -output -xp=maqao-ole-DATE\n")

   -- Add OoO file decoration
   print (". UFS OoO file decoration with duplication\n")
   print ("  # maqao ole -ooo -OoO=OOO_FILE -t=OLE_GROUP_TRACE -d=DUPLICATION FACTOR\n")

   print (". Help\n")
   print ("  # maqao -h") 
   print ("or")
   print ("  # maqao \n")

   print("\n/!\\ WARNING: this version targets only innermost loops and can only handle one loop at a time.\n")
   
   os.exit(0)
end

-- Generate experiment path (TODO: allow user to specify a path)
local function ole_gen_xp()
   
   local timestamp = os.date("*t")
   local date = timestamp.day.."-"..timestamp.month.."-"..timestamp.year
   local time = timestamp.hour..":"..timestamp.min..":"..timestamp.sec
   local xp_str = "maqao-ole_"..date.."/" --"_"..time.."/"

   -- Avoid time in order to simplify batch processing
  
   -- Check and create
   if (fs.exists(xp_str))
   then -- TODO: Handle this case smartly!
      return nil, "ERR_XP_EXISTS"
   else
      if (lfs.mkdir(xp_str) == false)
      then 
      	 return nil, "ERR_XP_FAIL"
      else
	 lfs.mkdir(xp_str.."bin")  -- Binary directory
	 lfs.mkdir(xp_str.."cfg")  -- Configuration directory
	 lfs.mkdir(xp_str.."out")  -- Output directory 
	 lfs.mkdir(xp_str.."out/pix") -- HTML report pictures
	 lfs.mkdir(xp_str.."meta") -- Static analysis directory

	 -- Copy images in out/pix/
      end
   end
   
   return xp_str, nil
end

-- Generate a configuration file template
local function ole_gen_cfg_temp(xp)

   -- Assume file name and location (PARAMETERIZE)
   local cfg = io.open(xp.."cfg/cfg.lua", "wb")
   
   cfg:write("nbcores = 8\n"..
		"\n"..
		"L1 = {\n"..
		"   size = 32 * 1024,\n"..
		"   cost = 2,\n"..
		"   level = 1,\n"..
		"   assoc = 8,\n"..
		"   linesize = 64,\n"..
		"   policy = \"BLRU\"\n"..
		"}\n"..
		"\n"..
		"L2 = {\n"..
		"   size = 256 * 1024,\n"..
		"   cost = 5,\n"..
		"   level = 2,\n"..
		"   assoc = 8,\n"..
		"   linesize = 64,\n"..
		"   policy = \"BLRU\"\n"..
		"}\n"..
		"\n"..
		"L3 = {\n"..
		"   size = 8 * 1024 * 1024,\n"..
		"   cost = 47,\n"..
		"   level = 3,\n"..
		"   assoc = 16,\n"..
		"   linesize = 64,\n"..
		"   policy = \"BLRU\"\n"..
		"}\n")
   
   cfg:close()
end

--
local function ole_get_param_loops_lst(loops)
   return String:get_tokenized_table(loops, ',', false, false, nil)
end

--
local function ole_get_param_funcs_lst(funcs)
   return String:get_tokenized_table(funcs, ',', false, false, nil)
end

--
local function ole_get_param_insns_lst(insns)
   return String:get_tokenized_table(insns, ',', false, false, nil)
end

--
local function ole_chk_param(args)
  
   local params = {}
   local binary, blocks, block_type, output, trace, ooo, dup, xp
      
   -- OOO generation mode
   if (args.ooo ~= nil)
   then
      
      if (args.OoO and args.OoO ~= nil) -- trace file
      then
	 
	 if (fs.exists(args.OoO))
	 then
	    ooo = args.OoO
	 else
	    return nil, "ERR_FILE_NOTFOUND"
	 end	 
      else
	 return nil, "ERR_PARAM"
      end

      if (args.t and args.t ~= nil) -- trace file
      then
	 
	 if (fs.exists(args.t))
	 then
	    trace = args.t
	 else
	    return nil, "ERR_FILE_NOTFOUND"
	 end 
      else
	 return nil, "ERR_PARAM"
      end

      if (args.d and args.d ~= nil) -- trace file
      then
	 dup = args.d
      else
	 return nil, "ERR_PARAM"
      end
      
      return "ooo", { ooo = ooo, trace = trace, dup = dup } 
   else
      if (args.instru)
      then 
	 if (args.b and args.b ~= nil) -- Instrumentation
	 then
	    
	    if (fs.exists(args.b))
	    then
	       binary = args.b
	    else
	       return nil, "ERR_BIN_NOTFOUND"
	    end
	    
	    if (args.l and args.l ~= nil)
	    then
	       block_type = "loops"
	       blocks = ole_get_param_loops_lst(args.l)
	    else
	       if (args.f and args.f ~= nil)
	       then
		  block_type = "functions"
		  blocks = ole_get_param_funcs_lst(args.f)
	       else
		  if (args.i and args.i ~= nil)
		  then 
		     block_type = "instructions"
		     blocks = ole_get_param_insns_lst(args.i)
		  else
		     return nil, "ERR_PARAM"
		  end
	       end
	    end
	    
	    return "instru", { binary = binary, block_type = block_type, blocks = blocks }  
	 end
      elseif (args.output)
      then
	 
	 if (args.xp and args.xp ~= nil)
	 then
	    
	    xp = args.xp.."/"
	    
	 else
	    return nil, "ERR_PARAM"
	 end
	 
	 return "output", { xp = xp }
      else
	 if (args.h or args.help)
	 then
	    return "help", nil
	 else
	    return "help", nil
	 end
      end
   end
end

-- Module entry point
function ole:ole_launch(args, proj)
   
   local mode, params = ole_chk_param(args)

   math.randomseed(os.time())
   
   if (mode == "instru")
   then 
      
      xp, err = ole_gen_xp()
      
      if (xp)
      then 
	 
	 print(ole_banner[math.random(1, 3)])
	 
	 print ("OLE_info : Generating experiment path "..xp)
	 ole_gen_cfg_temp(xp)
	 
	 local ole_instru_proj = project.new("ole instru")
	 local asmfile = ole_instru_proj:load(params.binary)
	 local madras_obj = madras.new(params.binary)   -- Instrumentation object
	 
	 madras_obj:modifs_init(MDSAPI.STACK_SHIFT, 512)
	 madras_obj:traceon()
	       
	 print ("OLE_info: Instrumenting target ".. params.block_type.." ...")
	 
	 if (params.block_type == "loops")
	 then 
	    
	    local loops_meta, err = ole:ole_analyze_loops(params.blocks, asmfile, ole_instru_proj, xp)
	    
	    if (loops_meta)
	    then
	       
	       if (err == 0) 
	       then
	       	  ole:ole_print_error("ERR_NOLOOP_PROCESSED", xp)
	       end
	       
	       -- Dump meta analysis 
	       ole:ole_dump_loops_meta(loops_meta, params.binary, xp)
	       
	       -- Instrument en generate binary
	       ole:ole_instrument_binary(params.block_type, loops_meta, asmfile, madras_obj, params.binary, xp)
	    else
	       ole:ole_print_error(err)
	    end
	 else
	    if (params.block_type == "functions")
	    then
	       ole:ole_print_error("ERR_UNHNDLD_FEATURE", xp)
	    else
	       if (params.block_type == "instructions")
	       then
		  
		  local insns_meta, err = ole:ole_analyze_insns(params.blocks, asmfile, ole_instru_proj)
		  
		  if (insns_meta)
		  then
		     
		     -- if (err == 0) 
		     -- then
		     ole:ole_print_error("ERR_NOLOOP_PROCESSED", xp)
		     -- end
		     
		     ole:ole_dump_insns_meta(insns_meta, params.binary, xp)

		     -- Instrument and generate binary
		     ole:ole_instrument_binary(params.block_type, insns_meta, asmfile, madras_obj, params.binary, xp)
		  else
		     ole:ole_print_error(err, xp)
		  end
	       end
	    end
	 end
	 
      else
	 ole:ole_print_error(err, xp)
      end      
   else
      if (mode == "ooo") -- ooo file generation
      then
	 
	 local ooo_out_fname = params.ooo:gsub(".ooo", "-U"..params.dup..".ooo")
	 	 
	 --
	 ole:ole_generate_decorated_OoO_file(ooo_out_fname, params.ooo, params.trace, params.dup)
	 
      else
	 if (mode == "output")
	 then

	    if (fs.exists(params.xp))
	    then
	       ole:ole_postproc_csv(params.xp)
	       ole:ole_stats_to_html(params.xp, 1)
	    else
	       ole:ole_print_error("ERR_XP_NOT_FOUND", nil)
	    end
	 else
	    if (mode == "help")
	    then
	       ole_usage();
	    else
	       ole:ole_print_error(params, nil)
	    end
	 end
	 
      end
   end
   
   os.exit(0)
end
