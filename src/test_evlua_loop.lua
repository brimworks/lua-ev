
local tap   = require("tap")
local evlua = require("evlua")
local help  = require("test_help")
local ok    = tap.ok

local noleaks = help.collect_and_assert_no_watchers
local loop = evlua.default

ok(type(evlua.version()) == "number",
   "version=" .. tostring(evlua.version()));

ok(type(select(2, evlua.version())) == "number",
   "minor_version=" .. tostring(select(2, evlua.version())));
