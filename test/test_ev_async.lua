print '1..17'

local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

local tap   = require("tap")
local ev = require("ev")
local help  = require("help")
local dump  = require("dumper").dump
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers
local loop = ev.Loop.default

-- Simply see if async watchers work at all:
function test_basic()
   local idle1 = ev.Async.new(
      function(loop, async, revents)
         ok(true, 'async')
         ok(ev.ASYNC == revents, 'ev.ASYNC(' .. ev.ASYNC .. ') == revents (' .. revents .. ')')
         async:stop(loop)
      end)
   idle1:start(loop)
   local timer1 = ev.Timer.new(
     function(loop, timer, revents)
       idle1:trigger(loop)
       timer:stop(loop)
     end, 0.1)
   timer1:start(loop)
   loop:loop()
end

noleaks(test_basic, "test_basic")

