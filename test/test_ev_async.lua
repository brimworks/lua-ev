print '1..20'

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

function test_basic()
   local async1 = ev.Async.new(
      function(loop, async, revents)
         ok(true, 'async callback')
         ok(ev.ASYNC == revents, 'ev.ASYNC(' .. ev.ASYNC .. ') == revents (' .. revents .. ')')
      end)
   async1:start(loop)
   async1:send(loop)
   loop:loop()
end

function test_export()
   local async1 = ev.Async.new(
      function(loop, async, revents)
         ok(true, 'async callback')
         ok(ev.ASYNC == revents, 'ev.ASYNC(' .. ev.ASYNC .. ') == revents (' .. revents .. ')')
      end)
   local send = ev.Async.import(ev.Async.export(async1, loop))
   async1:start(loop)
   send()
   loop:loop()
end

noleaks(test_basic, "test_basic")
noleaks(test_basic, "test_export")
