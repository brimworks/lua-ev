local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

local tap   = require("tap")
local ev    = require("ev")
local help  = require("help")
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers

local num = ev.Loop.default:iteration()
ok(num == 0,
   "iteration=" .. num)

num = ev.Loop.default:depth()
ok(num == 0,
   "depth=" .. num)

-- Shouldn't do anything:
ev.Loop.default:fork()

-- Do one event loop:
ev.Loop.default:loop()

-- Be sure old API still works:
num = ev.Loop.default:count()
ok(num == 1,
   "iteration=" .. num)

ok(type(ev.version()) == "number",
   "version=" .. tostring(ev.version()));

ok(type(select(2, ev.version())) == "number",
   "minor_version=" .. tostring(select(2, ev.version())));

ok(type(ev.Loop.default:backend()) == "number",
   "default backend  = " .. tostring(ev.Loop.default:backend()))

ok(ev.Loop.new(1):backend() == 1,
   "Able to choose backend 1 (select), may fail if LIBEV_FLAGS environment variable is set")

ok(ev.Loop.new(2):backend() == 2,
   "Able to choose backend 2 (poll), fails on windows or if LIBEV_FLAGS environment variable excludes this backend")
