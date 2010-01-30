local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

local tap   = require("tap")
local ev    = require("ev")
local help  = require("help")
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers
local loop = ev.default

ok(type(ev.version()) == "number",
   "version=" .. tostring(ev.version()));

ok(type(select(2, ev.version())) == "number",
   "minor_version=" .. tostring(select(2, ev.version())));
