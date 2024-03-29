# lua-ev

## Requirements

* [libev](http://dist.schmorp.de/libev/):
[mirror](http://software.schmorp.de/pkg/libev.html)
* [CMake](http://www.cmake.org/cmake/resources/software.html) for building

## Loading the library

* If you built the library as a loadable package
```lua
[local] ev = require 'ev'
```
* If you compiled the package statically into your application, call
the function `luaopen_ev(L)`. It will create a table with the ev
functions and leave it on the stack.

## Choosing a backend:

Set the `LIBEV_FLAGS=` environment variable to choose a backend:

1. select()
2. poll()
4. epoll()
8. kqueue
16. /dev/poll (not implemented)
32. Solaris port

Please see the documentation for libev for more details.

**WARNING**:

If your program `fork()`s, then you will need to re-initialize
your event loop(s) in the child process.  You can do this
re-initialization using the `loop:fork()` function.

**NOTE**:

If you are new to event loop programming, take a look at
[example.lua](example.lua).

## ev functions

### major, minor = ev.version()

returns numeric ev version for the major and minor
levels of the version dynamically linked in.

### loop = ev.Loop.new()

Create a new non-default event loop.  See ev.Loop object methods
below.

### loop = ev.Loop.default

The "default" event loop.  See ev.Loop object methods below.
Note that the default loop is "lazy loaded".

### timer = ev.Timer.new(on_timeout, after_seconds [, repeat_seconds])

Create a new timer that will call the on_timeout function when the
timer expires.  The timer initially expires in after_seconds
(floating point), then it will expire in repeat_seconds (floating
point) over and over again (unless repeat_seconds is zero or is
omitted in which case, this timer will only fire once).

The returned timer is an ev.Timer object.  See below for the
methods on this object.

**NOTE**: You must explicitly register the timer with an event loop in
order for it to take effect.

The on_timeout function will be called with these arguments
(return values are ignored):

### on_timeout(loop, timer, revents)

The loop is the event loop for which the timer is registered,
the timer is the ev.Timer object, and revents is ev.TIMEOUT.

See also `ev_timer_init()` C function.

### sig = ev.Signal.new(on_signal, signal_number)

Create a new signal watcher that will call the on_signal function
when the specified signal_number is delivered.

The returned sig is an ev.Signal object. See below for the methods on
this object.

**NOTE**: You must explicitly register the sig with an event loop in
order for it to take effect.

The on_signal function will be called with these arguments (return
values are ignored):

### on_signal(loop, sig, revents)

The loop is the event loop for which the io object is
registered, the sig parameter is the ev.Signal object, and
revents is ev.SIGNAL.

See also `ev_signal_init()` C function.

### io = ev.IO.new(on_io, file_descriptor, revents)

Create a new io watcher that will call the on_io function when the
specified file_descriptor is available for read and/or write
depending on the revents.  The revents parameter must be either
ev.READ, ev.WRITE, or the bitwise or of ev.READ and ev.WRITE (use
bitlib to do bitwise or).

The returned io is an ev.IO object.  See below for the methods on
this object.

**NOTE**: You must explicitly register the io with an event loop in
order for it to take effect.

The on_io function will be called with these arguments (return
values are ignored):

### on_io(loop, io, revents)

The loop is the event loop for which the io object is
registered, the io parameter is the ev.IO object, and revents
is a bit set consisting of ev.READ and/or ev.WRITE and/or
ev.TIMEOUT depending on which event triggered this callback.
Of course ev.TIMEOUT won't be in that set since this is the io
watcher.

See also `ev_io_init()` C function.

### idle = ev.Idle.new(on_idle)

Create a new io watcher that will call the on_idle function
whenever there is nothing else to do.  This means that the loop
will never block while an idle watcher is started.

The returned io is an ev.Idle object.  See below for the methods on
this object.

**NOTE**: You must explicitly register the idle with an event loop in
order for it to take effect.

The on_idle function will be called with these arguments (return
values are ignored):

### on_idle(loop, idle, revents)

The loop is the event loop for which the idle object is
registered, the idle parameter is the ev.Idle object, and
revents is ev.IDLE.

See also `ev_idle_init()` C function.

### async = ev.Async.new(on_async)

Create a new async watcher that will call the on_async function
whenever an application calls ev_async_send() from another context
(this can be another thread or some other context which does not
control the loop this watcher lives on)

The returned async is an ev.Async object.  See below for the methods
on this object.

NOTE: You must explicitly register the async with an event loop in
order for it to take effect.

The on_async function will be called with these arguments (return
values are ignored):

For a LuaJIT/FFI helper about the use of ev_async_send() from another thread, see [contrib/ev-async.lua](contrib/ev-async.lua).

### on_async(loop, idle, revents)

The loop is the event loop for which the idle object is
registered, the idle parameter is the ev.Idle object, and
revents is ev.IDLE.

See also `ev_idle_init()` C function.

### child = ev.Child.new(on_child, pid, trace)

Create a new child watcher that will call the on_child function
whenever SIGCHLD for registered pid is delivered.

When pid is set to 0 the watcher will fire for any pid.

When trace is false the watcher will be activated when the process
terminates. If it's true - it will additionally be activated when
the process is stopped or continued.

The returned child is an ev.Child object.  See below for the methods on
this object.

NOTE: You must explicitly register the idle with an event loop in
order for it to take effect.

The on_child function will be called with these arguments (return
values are ignored):

### on_child(loop, child, revents)

The loop is the event loop for which the idle object is
registered, the child parameter is the ev.Child object, and
revents is ev.CHILD.

See also `ev_child_init()` C function.

### stat = ev.Stat.new(on_stat, path [, interval])

Configures the watcher to wait for status changes of the given "path".
The "interval" is a hint on how quickly a change is expected to be
detected and may normally be left out to let libev choose a suitable
value.

The returned stat is an ev.Stat object.  See below for the methods on
this object.

NOTE: You must explicitly register the stat with an event loop in
order for it to take effect.

The on_stat function will be called with these arguments (return
values are ignored):

### on_stat(loop, stat, revents)

The loop is the event loop for which the idle object is
registered, the stat parameter is the ev.Stat object, and
revents is ev.STAT.

See also `ev_stat_init()` C function.

### ev.READ (constant)

If this bit is set, the io watcher is ready to read. See also
`EV_READ` C definition.

### ev.WRITE (constant)

If this bit is set, the io watcher is ready to write. See also
`EV_WRITE` C definition.

### ev.TIMEOUT (constant)

If this bit is set, the watcher was triggered by a timeout. See
also `EV_TIMEOUT` C definition.

### ev.SIGNAL (constant)

If this bit is set, the watcher was triggered by a signal. See
also `EV_SIGNAL` C definition.

### ev.ASYNC (constant)

If this bit is set, the watcher has been asynchronously notified. See also
`EV_ASYNC` C definition.

### ev.CHILD (constant)

If this bit is set, the watcher was triggered by a child signal.
See also `EV_CHILD` C definition.

### ev.STAT (constant)

If this bit is set, the watcher was triggered by a change in
attributes of the file system path. See also `EV_STAT` C definition.

## ev.Loop object methods

### loop:fork()

You *must* call this function in the child process after `fork(2)`
system call and before the next iteration of the event loop.

### loop:loop()

Run the event loop!  Returns when there are no more watchers
registered with the event loop.  See special note below about
calling ev_loop() C API.

See also `ev_loop()` C function.

### bool = loop:is_default()

Returns true if the referenced loop object is the default event
loop.

See also `ev_is_default_loop()` C function.

### num = loop:iteration()

Returns the number of loop iterations.  Note that this function
used to be called loop:count().

See also `ev_iterations()` C function.

### num = loop:depth() [libev >= 3.7]

Returns the number of times loop:loop() was entered minus the
number of times loop:loop() was exited, in other words, the
recursion depth.

This method is available only if lua-ev was linked
with libev 3.7 and higher.

See also `ev_depth()` C function.

### epochs = loop:now()

Returns the non-integer epoch seconds time at which the current
iteration of the event loop woke up.

See also `ev_now()` C function.

### epochs = loop:update_now()

Updates the current time returned by loop.now(), and returns that
timestamp.

See also `ev_now_update()` C function.

### loop:unloop()

Process all outstanding events in the event loop, but do not make
another iteration of the event loop.

See also `ev_unloop()` C function.

### backend_id = loop:backend()

Returns the identifier of the current backend which is being used
by this event loop.  See the libev documentation for what each
number means:

http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#FUNCTIONS_CONTROLLING_THE_EVENT_LOOP

## object methods common to all watcher types

### bool = watcher:is_active()

Returns true if the watcher is active (has been start()ed, but not
stop()ed).

See also `ev_is_active()` C function.

### bool = watcher:is_pending()

Returns true if the watcher is pending (it has outstanding events
but its callback has not yet been invoked).

See also `ev_is_pending()` C function.

### revents = watcher:clear_pending()

If the watcher is pending, return the revents of the pending
event, otherwise returns zero.  If the event was pending, the
pending flag is cleared (and therefore this watcher event will not
trigger the events callback).

See also `ev_clear_pending()` C function.

### old_priority = watcher:priority([new_priority])

Get access to the priority of this watcher, optionally setting a
new priority.  The priority should be a small integer between
ev.MINPRI and ev.MAXPRI.  The default is 0.

See also the `ev_priority()` and `ev_set_priority()` C functions.

### old_callback = watcher:callback([new_callback])

Get access to the callback function associated with this watcher,
optionally setting a new callback function.

## ev.Timer object methods

### timer:start(loop [, is_daemon])

Start the timer in the specified event loop.  Optionally make this
watcher a "daemon" watcher which means that the event loop will
terminate even if this watcher has not triggered.

See also `ev_timer_start()` C function (document as `ev_TYPE_start()`).

### timer:stop(loop)

Unregister this timer from the specified event loop.  Ensures that
the watcher is neither active nor pending.

See also `ev_timer_stop()` C function (document as `ev_TYPE_stop()`).

### timer:again(loop [, seconds])

Reset the timer so that it doesn't trigger again in the specified
loop until the specified number of seconds from now have elapsed.
If seconds is not specified, uses the repeat_seconds specified
when the timer was created.

See also `ev_timer_again()` C function.

## ev.IO object methods

### io:start(loop [, is_daemon])

Start the io in the specified event loop.  Optionally make this
watcher a "daemon" watcher which means that the event loop will
terminate even if this watcher has not triggered.

See also `ev_io_start()` C function (document as `ev_TYPE_start()`).

### io:stop(loop)

Unregister this io from the specified event loop.  Ensures that
the watcher is neither active nor pending.

See also `ev_io_stop()` C function (document as `ev_TYPE_stop()`).

### fd = io:getfd()

Returns the file descriptor associated with the IO object.

## ev.Idle object methods

idle:start(loop [, is_daemon])

Start the idle watcher in the specified event loop.  Optionally
make this watcher a "daemon" watcher which means that the event
loop will terminate even if this watcher has not triggered.

See also `ev_idle_start()` C function (document as `ev_TYPE_start()`).

### idle:stop(loop)

Unregister this idle watcher from the specified event loop.
Ensures that the watcher is neither active nor pending.

See also `ev_io_stop()` C function (document as `ev_TYPE_stop()`).

## ev.Async object methods

### async:start(loop [, is_daemon])

Start the async watcher in the specified event loop.  Optionally
make this watcher a "daemon" watcher which means that the event
loop will terminate even if this watcher has not triggered.

See also `ev_async_start()` C function (document as `ev_TYPE_start()`).

### async:stop(loop)

Unregister this async watcher from the specified event loop.
Ensures that the watcher is neither active nor pending.

See also `ev_async_stop()` C function (document as `ev_TYPE_stop()`).

### async:send(loop)

Sends/signals/activates the given "ev_async" watcher, that is, feeds an
"EV_ASYNC" event on the watcher into the event loop, and instantly returns.

See also `ev_async_send()` C function.

## ev.Child object methods

### child:start(loop [, is_daemon])

Start the child watcher in the specified event loop.  Optionally
make this watcher a "daemon" watcher which means that the event
loop will terminate even if this watcher has not triggered.

See also `ev_child_start()` C function (document as `ev_TYPE_start()`).

### child:stop(loop)

Unregister this child watcher from the specified event loop.
Ensures that the watcher is neither active nor pending.

See also `ev_child_stop()` C function (document as `ev_TYPE_stop()`).

### child:getpid()

Return the process id this watcher watches out for, or 0, meaning
any process id.

### child:getrpid()

Return the process id that detected a status change.

### child:getstatus()

Returns the process exit/trace status caused by "rpid" (see your systems
"waitpid" and "sys/wait.h" for details).

It returns the table with the following fields:

- `exited`: true if status was returned for a child process that terminated
normally;
- `stopped`: true if status was returned for a child process that is
currently stopped;
- `signaled`: true if status was returned for a child process that terminated
due to receipt of a signal that was not caught;
- `exit_status`: (only if exited == true) the low-order 8 bits of the status
argument that the child process passed to _exit() or exit(), or the value
the child process returned from main();
- stop_signal`: (only if stopped == true) the number of the signal that
caused the child process to stop;
- `term_signal`: (only if signaled == true) the number of the signal that
caused the termination of the child process.

## ev.Stat object methods

### stat:start(loop [, is_daemon])

Start the stat watcher in the specified event loop.  Optionally
make this watcher a "daemon" watcher which means that the event
loop will terminate even if this watcher has not triggered.

See also `ev_stat_start()` C function (document as `ev_TYPE_start()`).

### stat:stop(loop)

Unregister this stat watcher from the specified event loop.
Ensures that the watcher is neither active nor pending.

See also `ev_stat_stop()` C function (document as `ev_TYPE_stop()`).

### stat:getdata()

Returns a table with the following fields:

* - path: the file system path that is being watched;
* - interval: the specified interval;
* - attr: the most-recently detected attributes of the file in a form
*   of table with the following fields: dev, ino, mode, nlink, uid, gid,
*   rdev, size, atime, mtime, ctime corresponding to struct stat members
*   (st_dev, st_ino, etc.);
* - prev: the previous attributes of the file with the same fields as
*   attr fields.

### EXCEPTION HANDLING NOTE

If there is an exception when calling a watcher callback, the error
will be printed to stderr.  In the future, it would be cool if
there was a linked list of error handlers, and that if a callback
registers another callback, this linked list of error handlers
would be inherited so that exception handling can be done more
easily.  To do this, we just need to add a method for adding an
error handler for the current callback context, and keep calling
the error handlers in the linked list until an error handler
actually handles the exception.  If the error handling stack
unwinds, we will probably just resort to printing to stderr.

### CALLING ev_loop() C API DIRECTLY:

If you want to call the `ev_loop()` C API directly, then you **MUST**
set the loop userdata field to the `lua_State` in which you want all
callbacks to be ran in.  In the past, the `lua_State` was stored as
a thread local which was set when `loop:loop()` lua function is
called, but this made lua-ev dependent upon pthreads and forced
`ev_loop()` to only be called from lua.  Next, I removed the pthreads
dependency and stored the `lua_State` in which the callback was
registered.  This doesn't bode well when working with coroutines as
it means that `loop:loop()` may call back into a coroutine that is
long gone.

The current implementation relies on libev 3.7's ability to set a
userdata field associated with an event loop, and the loop:loop()
implementation simply sets the userdata field for the duration of
`loop:loop()`. This does mean that if you want to call `ev_loop()`
from C then you either need to be sure that no lua watchers are
registered with that loop, or you need to set the userdata field to
the `lua_State*` in which the callbacks should be ran.

## TODO

* [ ] Add support for other watcher types (periodic, embed, etc).

[MIT License](LICENSE)
