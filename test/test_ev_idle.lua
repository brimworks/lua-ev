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

-- Simply see if idle watchers work at all:
function test_basic()
   local idle1 = ev.Idle.new(
      function(loop, idle, revents)
         ok(true, 'simple idle')
         ok(ev.IDLE == revents, 'ev.IDLE(' .. ev.IDLE .. ') == revents (' .. revents .. ')')
         idle:stop(loop)
      end)
   idle1:start(loop)
   loop:loop()
end

-- See if idle prorities make sense
function test_priority()
   local high_count, low_count = 0, 0
   local idle_high = ev.Idle.new(
      function(loop, idle, revents)
         high_count = high_count + 1
         ok(low_count == 0, 'high idle running first')
         if high_count == 3 then
            idle:stop(loop)
         end
      end)
   ok(idle_high:priority(ev.MAXPRI) == 0, 'priority was default (0)')
   ok(idle_high:priority() == ev.MAXPRI, 'priority correctly set')
   idle_high:start(loop)
   local idle_low = ev.Idle.new(
      function(loop, idle, revents)
         low_count = low_count + 1
         ok(high_count == 3, 'low idle running last')
         if low_count == 3 then
            idle:stop(loop)
         end
      end)
   idle_low:start(loop)
   local daemon_count = 0
   local idle_daemon = ev.Idle.new(
      function(loop, idle, revents)
         daemon_count = daemon_count + 1
         ok(false, "daemon idle shouldn't run at all")
     end)
   ok(idle_daemon:priority(ev.MINPRI) == 0, 'priority was default (0)')
   ok(idle_daemon:priority() == ev.MINPRI, 'priority correctly set')
   idle_daemon:start(loop, true)
   loop:loop()
   ok(high_count == 3, 'high idle ran thrice')
   ok(low_count == 3, 'low idle ran thrice')
   ok(daemon_count == 0, 'daemon idle never ran')
   idle_daemon:stop(loop)
end

function test_shadow_table()
   local idle = ev.Idle.new(
      function(loop, idle, revents)
         idle:stop(loop)
         ok(idle.user_data == "foo", 'shadow table works in callback')
      end)
   idle:start(loop)
   idle.user_data = "foo"
   ok(idle.user_data == "foo", 'shadow table works')
   loop:loop()
end

noleaks(test_basic, "test_basic")
noleaks(test_priority, "test_priority")
noleaks(test_shadow_table, "test_shadow_table")
