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

function test_basic()
    local pid
    local child_called = false
    local child = ev.Child.new(function(loop, child, revents)
        child_called = true
        local status = child:getstatus()
        ok(child:getrpid() == pid, 'got proper pid')
        ok(child:getpid() == 0, 'pid == 0')
        ok(status.exited, 'process exited')
        ok(status.exit_status == 0, 'process exited with exit status == 0')
        ok(status.stopped == false, 'process not stopped')
        ok(status.signaled == false, 'process not signaled')
        child:stop(loop)
    end, 0, false)
    child:start(loop)
    pid = io.popen("echo $$", "r"):read("*n")
    ok(pid > -1, 'fork successful')
    loop:loop()
    ok(child_called, "child forked")
end

noleaks(test_basic, "test_basic")
