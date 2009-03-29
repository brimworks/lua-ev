print '1..??'

local tap   = require("tap")
local evlua = require("evlua")
local help  = require("test_help")
local ok    = tap.ok


local noleaks = help.collect_and_assert_no_watchers
local loop    = evlua.Loop.default

-- Simply see if we can do a simple one second timer:
function test_basic() 
   local io1 =
      evlua.IO.new(function(timer)
                        ok(true, 'one second timer')
                     end, 1)
   io1:start(loop)
   loop:loop()
end

-- TODO: Need to deal wit file descriptors.
noleaks(test_basic(), "test_basic");
