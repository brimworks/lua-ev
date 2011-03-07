local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

local tap   = require("tap")
local ev    = require("ev")
local help  = require("help")
local dump  = require("dumper").dump
local ok    = tap.ok
local alien = require("alien")

local noleaks = help.collect_and_assert_no_watchers
local loop    = ev.Loop.default

function spawn(cmd)
    local fork = alien.default.fork
    local exec = alien.default.execl

    cmd = cmd or 'echo -e ""'

    fork:types('int')
    exec:types('int', 'string', 'string', 'string', 'string', 'string')
    
    local pid = fork()

    if (pid == 0) then
        local ret = exec('/bin/sh', '/bin/sh', '-c', cmd, nil)
        ok(not ret, 'shouldn\'t get here, ret: ' .. tostring(ret))
    else
        return pid
    end
end

function test_basic()
    local pid
    local child = ev.Child.new(function(loop, child, revents)
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
    pid = spawn()
    ok(pid > -1, 'fork successful')
    loop:loop()
end

noleaks(test_basic, "test_basic")
