local src_dir, build_dir = ...
package.path  = src_dir .. "?.lua;" .. package.path
package.cpath = build_dir .. "?.so;" .. package.cpath

-- This test relies on socket support:
local has_socket, socket = pcall(require, "socket")
if not has_socket then
   print '1..0'
   print('# SKIP: No socket library available (' .. socket .. ')')
   os.exit(0)
end
print '1..??'

local tap  = require("tap")
local ev   = require("ev")
local help = require("help")
local ok   = tap.ok
local dump = require("dumper").dump

local noleaks = help.collect_and_assert_no_watchers
local loop    = ev.Loop.default

local function test_stdin()
   local io1 = ev.IO.new(
      function(loop, io, revents)
         ok(true, 'STDIN is writable')
         ok(io:getfd() == 1, 'getfd() works')
         io:stop(loop)
      end, 1, ev.WRITE)
   io1:start(loop)
   loop:loop()
end

local function newtry()
   local try = {}
   setmetatable(try, try)
   function try:__call(body)
      local is_err, err = pcall(body)
      for _, finalizer in ipairs(self) do
         -- ignore errors in finalizers:
         pcall(finalizer)
      end
      assert(is_err, err)
   end
   function try:finally(finalizer)
      self[#self + 1] = finalizer
   end
   return try
end

local function test_echo()
   local got_response
   local try = newtry()
   try(function()
          local server = assert(socket.bind("*", 0))
          try:finally(function() server:close() end)
          server:settimeout(0)
          ev.IO.new(
             function(loop, watcher)
                watcher:stop(loop)
                local client = assert(server:accept())
                client:settimeout(0)
                ev.IO.new(
                   function(loop, watcher)
                      watcher:stop(loop)
                      local buff = assert(client:receive('*a'))
                      ev.IO.new(
                         function(loop, watcher)
                            watcher:stop(loop)
                            assert(client:send(buff))
                            assert(client:shutdown("send"))
                         end,
                         client:getfd(),
                         ev.WRITE):start(loop)
                   end,
                   client:getfd(),
                   ev.READ):start(loop)
             end,
             server:getfd(),
             ev.READ):start(loop)
          local port   = select(2, server:getsockname())
          local client = assert(socket.connect("127.0.0.1", port))
          try:finally(function() client:close() end)
          client:settimeout(0)
          ev.IO.new(
             function(loop, watcher)
                watcher:stop(loop)
                local str = "Hello World"
                assert(client:send(str))
                assert(client:shutdown("send"))
                ev.IO.new(
                   function(loop, watcher)
                      watcher:stop(loop)
                      local response = assert(client:receive("*a"))
                      ok(response == str,
                         tostring(response) .. " == " .. tostring(str))
                      got_response = true
                   end,
                   client:getfd(),
                   ev.READ):start(loop)
             end,
             client:getfd(),
             ev.WRITE):start(loop)
          loop:loop()
       end)
   ok(got_response, "echo")
end

noleaks(test_stdin, "test_stdin")
noleaks(test_echo,  "test_echo")

