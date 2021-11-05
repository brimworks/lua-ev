-- Extension for lua-ev and LuaJIT to trigger async watchers from other threads.
local ev = require("ev")
local ffi = require("ffi")

ffi.cdef[[
struct ev_loop;
struct ev_async;
void ev_async_send(struct ev_loop*, struct ev_async*);
]]
local libev = ffi.load("ev")
local loop_mt = getmetatable(ev.Loop.default)
local async_mt = getmetatable(ev.Async.new(function() end))
local async_sz_t = ffi.typeof("struct{struct ev_loop *loop; struct ev_async *async;}")

local function error_arg(index, expected)
  error("bad argument #"..index.." ("..expected.." expected)")
end

local M = {}

-- Export serialized send for multi-threading signalization.
-- return string
function M.export(loop, async)
  if getmetatable(loop) ~= loop_mt then error_arg(1, "loop") end
  if getmetatable(async) ~= async_mt then error_arg(2, "async watcher") end
  local loop_p = ffi.cast("struct ev_loop**", loop)[0]
  local async_p = ffi.cast("struct ev_async*", async)
  local async_sz = async_sz_t({loop = loop_p, async = async_p})
  return ffi.string(async_sz, ffi.sizeof(async_sz_t))
end

-- Import serialized send for multi-threading signalization.
-- Return a function which will act as `ev_async_send` when called.
--
-- Calling the function beyond the lifespan of either the exported loop or async
-- object will result in undefined behavior (e.g. crash/corruption). The same applies
-- with an ill-formed string.
function M.import(data)
  if type(data) ~= "string" or #data ~= ffi.sizeof(async_sz_t) then
    error("invalid payload")
  end
  local async_sz = async_sz_t()
  ffi.copy(async_sz, data, ffi.sizeof(async_sz_t))
  return function() libev.ev_async_send(async_sz.loop, async_sz.async) end
end

return M
