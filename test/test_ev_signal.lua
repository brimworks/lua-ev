print '1..17'

local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

local tap   = require("tap")
local ev    = require("ev")
local help  = require("help")
local dump  = require("dumper").dump
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers
local loop    = ev.Loop.default

-- Simply see if we can do a simple signal handler:
function test_basic()
   local sig = ev.Signal.new(
      function(loop, sig, revents)
         ok(true, 'got SIGALRM')
         ok(ev.SIGNAL == revents, 'ev.SIGNAL(' .. ev.SIGNAL .. ') == revents (' .. revents .. ')')
         sig:stop(loop)
      end,
      14) -- SIGALRM
   sig:start(loop)
   os.execute('kill -14 $PPID')
   loop:loop()
end

-- Test daemon=true
function test_daemon_true()
   local loop_iters = 0
   local sig = ev.Signal.new(
      function(loop, sig)
         ok(loop_iters > 0, 'got SIGALRM after one loop iteration')
         sig:stop(loop)
      end,
      14) -- SIGALRM

   sig:start(loop, true)
   loop:loop() -- Should be a no-op.
   loop_iters = 1
   os.execute('kill -14 $PPID')
   loop:loop()
end

-- Test stop(), start(), and is_active()
function test_start_stop_active()
   local cnt = 0
   local sig = ev.Signal.new(
      function(loop, sig)
         cnt = cnt + 1
         ok(cnt == 1, 'get SIGALRM exactly once')
         sig:stop(loop)
      end,
      14) -- SIGALRM

   ok(not sig:is_active(), 'not active')

   sig:start(loop)

   ok(sig:is_active(), 'active')
   sig:stop(loop)

   loop:loop() -- no-op

   sig:start(loop)
   os.execute('kill -14 $PPID')

   loop:loop()

   ok(cnt == 1, 'SIGALRM callback got called')
end

-- Test invoke()
function test_callback()
   local cnt = 0
   local sig = ev.Signal.new(
      function()
         cnt = cnt + 1
         ok(cnt == 1, 'Signal callback called exactly once')
      end,
      14)

   sig:callback()()
   ok(cnt == 1, 'Signal callback actually got called')
end

-- Test is_pending()
function test_is_pending()
   local num_pending = 0
   local num_called  = 0
   local sig2
   local sig1 = ev.Signal.new(
      function(loop, sig)
         if ( sig2:is_pending() ) then
            num_pending = num_pending + 1
         end
         num_called = num_called + 1
         sig:stop(loop)
      end,
      14)

   sig1:start(loop)

   sig2 = ev.Signal.new(
      function(loop, sig)
         if ( sig1:is_pending() ) then
            num_pending = num_pending + 1
         end
         num_called = num_called + 1
         sig:stop(loop)
      end,
      14)

   sig2:start(loop)

   os.execute('kill -14 $PPID')
   loop:loop()

   ok(num_pending == 1, 'exactly one signal was pending')
   ok(num_called  == 2, 'both signal handlers got called')
end

-- Test clear_pending()
function test_clear_pending()
   local num_called  = 0
   local sig2
   local sig1 = ev.Signal.new(
      function(loop, sig)
         if ( sig2:is_pending() ) then
            sig2:clear_pending(loop)
         end
         num_called = num_called + 1
      end,
      14)
   sig1:start(loop)

   sig2 = ev.Signal.new(
      function(loop, sig)
         if ( sig1:is_pending() ) then
            sig1:clear_pending(loop)
         end
         num_called = num_called + 1
      end,
      14)
   sig2:start(loop)

   local timer = ev.Timer.new(
      function(loop, sig)
         loop:unloop()
      end,
      0.01)
   timer:start(loop)

   os.execute('kill -14 $PPID')
   loop:loop()
   sig1:stop(loop)
   sig2:stop(loop)

   ok(num_called  == 1, 'just one signal handler got called')
end


noleaks(test_basic, "test_basic")
noleaks(test_daemon_true, "test_daemon_true")
noleaks(test_start_stop_active, "test_start_stop_active")
noleaks(test_callback, "test_callback")
noleaks(test_is_pending, "test_is_pending")
noleaks(test_clear_pending, "test_clear_pending")
