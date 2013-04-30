module(..., package.seeall)

local function dump_impl(input, output, path, indent, done)
   if nil == input then
      table.insert(output, "nil")
   elseif "table" == type(input) then
      if done[input] then
         table.insert(output, done[input]);
      else
         done[input] = path
         table.insert(output, tostring(input))
         table.insert(output, " {\n");
         for key, value in pairs(input) do
            table.insert(output, string.rep("  ", indent + 1)) -- indent it
            table.insert(output, string.format("[%s] = ", tostring(key)))
            local kpath = string.format("%s[%s]", path, tostring(key))
            dump_impl(value, output, kpath, indent + 1, done)
            table.insert(output, "\n")
         end
         table.insert(output, string.rep("  ", indent)) -- indent it
         table.insert(output, "}");
      end
   elseif "string" == type(input) then
      table.insert(output, "\"")
      table.insert(output, input)
      table.insert(output, "\"");
   elseif "number" == type(input) or "boolean" == type(input) then
      table.insert(output, tostring(input))
   else
      table.insert(output, "[")
      table.insert(output, tostring(input))
      table.insert(output, "]");
   end
end

function dump(name, input)
   local output = { tostring(name) }
   table.insert(output, " = ")
   dump_impl(input, output, name, 0, {})
   return table.concat(output)
end
