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

-- Task scheduler using the EV event loop

-- luajit EV_Timers.lua


local ev = require'ev'
local loop = ev.Loop.default

-- build a timer event loop with a task defined as a callback
function buildTimer(number, interval)
    local timer = ev.Timer.new(function() -- define callback
            print(os.date(), "CB " .. number, "interval: " .. interval)
         end, interval, interval) -- timer (secs) loop callback strategy to let code continue to main loop statement
    timer:start(loop)
end
-- --------------------------------

buildTimer(1, 5.0) -- schedule task execution
buildTimer(2, 2.0) -- schedule task execution
buildTimer(3, 0.5) -- schedule task execution
buildTimer(4, 10.0) -- schedule task execution

loop:loop() -- main code must reach here to start loop 
