module(..., package.seeall);

local tap   = require("tap")
local ev    = require("ev")
local ok    = tap.ok

function collect_and_assert_no_watchers(test, test_name)
   collectgarbage("collect")
   local base = ev.object_count()

   -- The default event loop counts as an object:
   if base == 0 then base = 1 end

   test()
   collectgarbage("collect")
   local count =  ev.object_count()
   ok(count == base, 'no active watchers after ' .. test_name .. ' got: ' .. count .. ' expected: ' .. base)
end
