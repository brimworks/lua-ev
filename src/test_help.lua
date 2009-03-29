module(..., package.seeall);

local tap   = require("tap")
local evlua = require("evlua")
local ok    = tap.ok

function collect_and_assert_no_watchers(test, test_name)
   collectgarbage("collect")
   local base = evlua.object_count()
   test()
   collectgarbage("collect")
   local count =  evlua.object_count()
   ok(count == base, 'no active watchers after ' .. test_name .. ' got: ' .. count .. ' expected: ' .. base)
end
