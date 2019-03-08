--
module ("consts", package.seeall)

local ole_errors = {

   ["ERR_UNHNDLD_FEATURE"] = { err_msg = "Unhandled feature", err_code = "00#" }, 

   ["ERR_INSN_ILLEGAL"]    = { err_msg = "Illegal instruction", err_code = "E0" }, 
   ["ERR_INSN_NOTFOUND"]   = { err_msg = "Instruction not found", err_code = "E1" },
   ["ERR_LOOP_NOTFOUND"]   = { err_msg = "Cannot find loop", err_code = "E2" },
   ["ERR_XP_EXISTS"]       = { err_msg = "Experiment directory already exists", err_code = "E3" },
   ["ERR_XP_FAIL"]         = { err_msg = "Cannot create experiment directory", err_code = "E4" },
   ["ERR_PARAM"]           = { err_msg = "Wrong parameter", err_code = "E5" },
   ["ERR_BIN_NOTFOUND"]    = { err_msg = "Binary file cannot be found", err_code = "E6" },
   ["ERR_INIT_NOTFOUND"]   = { err_msg = "_init function not found in binary", err_code = "E7" },

   ["ERR_NOLOOP_PROCESSED"] = { err_msg = "No loop was processed in the binary", err_code = "E8" },
   ["ERR_FILE_NOTFOUND"]   = { err_msg = "Parameter file not found", err_code = "E9" },
   ["ERR_XP_NOT_FOUND"]    = { err_msg = "Cannot find experiment directory", err_code = "E10" }
}

local ole_warnings = {

   ["WARN_LOOP_NOGROUPS"]    = { warn_msg = "Loop ignored : No groups found in loop : ", warn_code = "W0" }, 
   ["WARN_INSN_DISCARDED"]   = { warn_msg = "Instruction ignore : No loops associated to instruction : ", warn_code = "W1" } 
}

--
local function ole_cleanup_error(xp)

   if (xp)
   then 
      if (fs.exists(xp))
      then
	 os.execute("rm -R "..xp)
      end
   end
   
   os.exit(-1)
end

--
function ole:ole_print_error(err, xp)
   print ("OLE_ERROR (#"..ole_errors[err].err_code..") : "..ole_errors[err].err_msg)

   if (xp) then
      ole_cleanup_error(xp)
   end
end

--
function ole:ole_print_warning(warn, param)
   
   if (param)
   then
      print ("OLE_WARNING (#"..ole_warnings[warn].warn_code..") : "..ole_warnings[warn].warn_msg..param)
   else
      print ("OLE_WARNING (#"..ole_warnings[warn].warn_code..") : "..ole_warnings[warn].warn_msg.."?")
   end
end

function ole:ole_trim(s)
  return s:match "^%s*(.-)%s*$"
end


--[[
Ordered table iterator, allow to iterate on the natural order of the keys of a
table.

Example:
]]

function __genOrderedIndex( t )
    local orderedIndex = {}
    for key in pairs(t) do
        table.insert( orderedIndex, key )
    end
    table.sort( orderedIndex )
    return orderedIndex
end

function orderedNext(t, state)
    -- Equivalent of the next function, but returns the keys in the alphabetic
    -- order. We use a temporary ordered key table that is stored in the
    -- table being iterated.

    local key = nil
    --print("orderedNext: state = "..tostring(state) )
    if state == nil then
        -- the first time, generate the index
        t.__orderedIndex = __genOrderedIndex( t )
        key = t.__orderedIndex[1]
    else
        -- fetch the next value
        for i = 1,table.getn(t.__orderedIndex) do
            if t.__orderedIndex[i] == state then
                key = t.__orderedIndex[i+1]
            end
        end
    end

    if key then
        return key, t[key]
    end

    -- no more value to return, cleanup
    t.__orderedIndex = nil
    return
end

function ole:orderedPairs(t)
    -- Equivalent of the pairs() function on tables. Allows to iterate
    -- in order
    return orderedNext, t, nil
end
