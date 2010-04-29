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

-- Simply see if we can do a simple one second timer:
function test_basic()
   local timer1 = ev.Timer.new(
      function(loop, timer, revents)
         ok(true, 'one second timer')
         ok(ev.TIMEOUT == revents, 'ev.TIMEOUT(' .. ev.TIMEOUT .. ') == revents (' .. revents .. ')')
      end,
      0.01)
   timer1:start(loop)
   loop:loop()
end

-- Test daemon=true on timer()
function test_daemon_true()
   local timer1_count = 0
   local timer1 = ev.Timer.new(
      function(loop, timer)
         timer1_count = timer1_count + 1
         ok(timer1_count == 1, 'once and only once')
      end, 0.1)
   timer1:start(loop)

   local timer2_count = 0
   local timer2 = ev.Timer.new(
      function(loop, timer)
         timer2_count = timer2_count + 1
         ok(timer2_count == 1, 'once and only once')
      end, 0.01)

   timer2:start(loop, true)

   loop:loop()

   local timer3 = ev.Timer.new(
      function(loop, timer)
         ok(false, 'Should never be called!')
      end, 0.01)

   timer3:start(loop, true)


   loop:loop()

   -- TODO: Should we make it so timer3 is automatically stopped if it is never executed in the event loop?
   timer3:stop(loop)
end

-- Test stop(), start(), and is_active()
function test_start_stop_active()
   local timer1_count = 0
   local timer1 = ev.Timer.new(
      function(loop, timer)
         timer1_count = timer1_count + 1
         ok(timer1_count == 1, 'once and only once')
      end, 0.01)

   ok(not timer1:is_active(), 'not active')

   timer1:start(loop)

   ok(timer1:is_active(), 'active')

   timer1:stop(loop)

   loop:loop()

   timer1:start(loop)

   loop:loop()
end

-- Test invoke()
function test_callback()
   local timer1_count1 = 0
   local timer1 = ev.Timer.new(
      function()
         timer1_count1 = timer1_count1 + 1
         ok(timer1_count1 == 1, 'once and only once A')
      end, 0.01)

   -- Test calling the callback manually:
   timer1:callback()()

   local timer1_count2 = 0

   -- Test setting the callback:
   timer1:callback(
      function(loop, timer)
         timer1_count2 = timer1_count2 + 1
         ok(timer1_count2 == 1, 'once and only once B')
      end)

   -- Register it and have it get called:
   timer1:start(loop)

   loop:loop()
end

-- Test is_pending()
function test_is_pending()
   local num_pending = 0
   local num_called  = 0
   local timer2
   local timer1 = ev.Timer.new(
      function(loop, timer)
         if ( timer2:is_pending() ) then
            num_pending = num_pending + 1
         end
         num_called = num_called + 1
      end, 0.01)

   timer1:start(loop)

   timer2 = ev.Timer.new(
      function(loop, timer)
         if ( timer1:is_pending() ) then
            num_pending = num_pending + 1
         end
         num_called = num_called + 1
      end, 0.01)

   timer2:start(loop)

   loop:loop()

   ok(num_pending == 1, 'exactly one timer was pending')
   ok(num_called  == 2, 'both timers got called')
end

-- Test clear_pending()
function test_clear_pending()
   local num_called = 0
   local timer2
   local timer1 = ev.Timer.new(
      function(loop, timer)
         if ( timer2:is_pending() ) then
            timer2:clear_pending(loop)
         end
         num_called = num_called + 1
      end, 0.01)

   timer1:start(loop)

   timer2 = ev.Timer.new(
      function(loop, timer)
         if ( timer1:is_pending() ) then
            timer1:clear_pending(loop)
         end
         num_called = num_called + 1
      end, 0.01)

   timer2:start(loop)

   loop:loop()

   ok(num_called == 1, 'exactly one timer was called')
end


noleaks(test_basic, "test_basic")
noleaks(test_daemon_true, "test_daemon_true")
noleaks(test_start_stop_active, "test_start_stop_active")
noleaks(test_callback, "test_callback")
noleaks(test_is_pending, "test_is_pending")
noleaks(test_clear_pending, "test_clear_pending")
--print(dump("registry", debug.getregistry()[1]));

-- test_is_pending()
-- noleaks("test_is_pending")
-- test_clear_pending()
-- noleaks("test_clear_pending")

