
local tap   = require("tap")
local ev = require("ev")
local help  = require("test_help")
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers
local loop = ev.default

ok(type(ev.version()) == "number",
   "version=" .. tostring(ev.version()));

ok(type(select(2, ev.version())) == "number",
   "minor_version=" .. tostring(select(2, ev.version())));
