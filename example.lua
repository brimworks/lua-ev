-- Copyright (c) 2011 by Ross Anderson <ross_j_anderson@yahoo.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.
----------------------------------------------------------------------
--
-- RUNNING THIS EXAMPLE:
--
--    lua example.lua
--
-- HOW IT WORKS:
--
--  This example attempts to walk you through a "typical" event loop
--  program.  In general, all event loop programs work by registering
--  callback functions with the event loop that are executed when a
--  condition is met and the event loop is executing.
--
--  In this example, we start by registering an "idol" callback:
--
--     ev.Idle.new(build_all_timers):start(ev.Loop.default)
--
--  This callback is ran whenever the event loop has nothing else to
--  do.  Note that this callback is not immediately executed when it
--  is registered with the event loop, but instead defers execution
--  until the event loop begins.  In this case, the event loop begins
--  executing when this line of code runs:
--
--     ev.Loop.default:loop()
--
--  So, after the event loop is running it immediately calls
--  build_all_timers with the event loop and idol watcher as
--  arguments.  The first thing we do is cancel this idol watcher with
--  this line of code:
--
--     idol_event:stop(loop)
--
--  If we do not cancel this idol watcher, then build_all_timers will
--  run again the next time that the event loop is idol.  Try
--  commenting this line out and see what happens :-).
--
--  In the next line of code we call build_timer which creates a timer
--  that fires every 5 seconds (since interval=5):
--
--     local timer = ev.Timer.new(callback, interval, interval)
--     timer:start(loop)
--
--  We then register other times that fire at various other intervals,
--  the smallest interval being every 0.5 seconds.
--
--  After build_all_timers finishes execution, the function returns
--  back to the event loop so that other callbacks can be executed.
--  In this example, there are 4 active "watchers" all waiting for
--  various time intervals.  Naturally, the smallest time interval
--  (callback "3" which fires every 0.5 seconds) will be called by the
--  event loop 3 or 4 times before callback "2" which fires ever 2
--  seconds has a chance to execute.
--
--  Eventually, after 10 seconds worth of timers getting fired is
--  done, we remove these "watchers" from the event loop and when no
--  more active watchers exist in the event loop, the event loop
--  returns:
--
--     ev.Loop.default:loop()
--     -- At last the above line of code completes.


local ev = require'ev'


-- build a timer event loop with a task defined as a callback
function build_timer(loop, number, interval)
    local i = 0
    local function callback(loop, timer_event)
        print(os.date(), "CB " .. tostring(number), "interval: " .. interval)
        i = i + interval
        if i >= 10 then
            timer_event:stop(loop)
        end
    end
    local timer = ev.Timer.new(callback, interval, interval)
    timer:start(loop)
    return timer
end

-- our "bootstrapping" callback
function build_all_timers(loop, idol_event)
    -- We have "bootstapped" into the event loop and therefore do not
    -- care about having this function execute when the event loop is
    -- idol:
    idol_event:stop(loop)

    print("Run build_all_timers callback")
    build_timer(loop, 1, 5.0)
    -- By making the 2 second timer a higher priority than the 0.5
    -- second timer, we should see the 0.5 timer execute 3 times
    -- before the 2.0 timer goes off insead of seeing it execute 4
    -- times.
    build_timer(loop, 2, 2.0):priority(ev.MAXPRI)
    build_timer(loop, 3, 0.5)
    build_timer(loop, 4, 10.0)
end

print("Register build_all_timers callback")
ev.Idle.new(build_all_timers):start(ev.Loop.default)

print("Run the event loop")
ev.Loop.default:loop()
