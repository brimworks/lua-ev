package = "lua-ev"
version = "1.0-1"

source = {
   url = "http://download.github.com/brimworks-lua-ev-7a4b879.tar.gz"
}

description = {
   summary = "Lua integration with libev",
   detailed = [[
      lua-ev: Lua integration with libev (http://dist.schmorp.de/libev)
   ]],
   homepage = "http://github.com/brimworks/lua-ev",
   license = "MIT/X11"
}

dependencies = {
   "lua >= 5.1"
}

external_dependencies = {
   LIBEV = {
      header = "ev.h",
      library = "libev.so"
   },
   PTHREADS = {
      header = "pthread.h",
   },
   platforms = {
      linux = {
         PTHREADS = {
           library = "libpthread.so"
         }
      }
   }
}

build = {
   type = "builtin",
   modules = {
      ev = {
         sources = {
            "lua_ev.c"
         },
         libraries = {
            "ev"
         }
      }
   },
   platforms = {
      linux = {
         modules = {
            ev = {
               libraries = {
                  "ev", "pthread"
               }
            }
         }
      }
   }
}
