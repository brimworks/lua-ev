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

function touch_file(path)
    ev.Timer.new(function(loop, timer, revents)
        os.execute('touch ' .. path)
        timer:stop(loop)
    end, 1):start(loop)
end

function remove_file(path)
    ev.Timer.new(function(loop, timer, revents)
        os.execute('rm ' .. path)
        timer:stop(loop)
    end, 1):start(loop)
end

function test_basic()
    local path = os.tmpname()
    local stat = ev.Stat.new(function(loop, stat, revents)
        local data = stat:getdata()
        ok(data.path == path, 'got proper path')
        ok(data.attr.nlink > 0, 'file exists')
        ok(data.attr.size == 0, 'file has size of 0 bytes')
        os.execute('rm ' .. path)
        stat:stop(loop)
    end, path)
    stat:start(loop)
    touch_file(path)
    loop:loop()
end

function test_remove()
    local path = os.tmpname()
    local stat = ev.Stat.new(function(loop, stat, revents)
        local data = stat:getdata()
        ok(data.path == path, 'got proper path')
        ok(data.attr.nlink == 0, 'file doesn\'t exist')
        ok(data.prev.nlink > 0, 'file previously existed')
        stat:stop(loop)
    end, path)
    stat:start(loop)
    remove_file(path)
    loop:loop()
end

noleaks(test_basic, "test_basic")
noleaks(test_remove, "test_remove")

