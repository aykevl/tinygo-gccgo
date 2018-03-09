# TinyGo

## What is this?

A runtime for the [Go programming language](https://golang.org/) that is very
small in size and doesn't rely as much on an OS.

**Note**: This project is unrelated to the now-dead [Tiny
runtime](https://code.google.com/archive/p/tinygo/) of the Go language, see
below.

## Why was it created?

I wanted to be able to run Go on a microcontroller. While this goal hasn't been
reached yet, the resulting binary can be _very_ small.

Inside `src/hello/hello.go`:

```go
// Simple program with the bare minimum of Go.
package main

func main() {
        println("hello, world!")
}
```

Running on an ARM machine (Raspberry Pi 3):

```
$ make PKG=hello LTO=1
[lots of build output]

$ strip build/hello

$ ./build/hello
hello, world!

$ /bin/ls -lh build/hello
-rwxrwxr-x 1 ayke ayke 5,5K mrt  9 23:43 build/hello

$ size build/hello
   text    data     bss     dec     hex filename
   1961     316      20    2297     8f9 build/hello
```

As you can see, this produces a file that's just 5.5kB in size and contains
about 2kB of machine code. The same source compiled by the official Go compiler
(1.7.4) produces an executable that is about 564kB in size after stripping, most
of which is also `.text`.

Of course, this is a bit cheating. The small 'hello' program does not include
most of the runtime, that's all stripped away by the linker. A somewhat more
realistic example is the
[channel](https://github.com/aykevl/tinygo/blob/master/src/channel/channel.go)
example, which produces an executable of 18kB. That runtime contains the
scheduler, memory allocator (no GC yet) and channel send/receive primitives.

## Limitations

There are many.

  * No support for anything except ARM.
  * Works with gccgo 6.3 (Debian stretch), no other compiler versions have been
    tested.
  * Source files must be placed in a subdirectory of src/.
  * No `recover()`
  * No function names or line numbers in the `panic()` output. I would like to
    fix this at some time, but it will increase the binary size if it must work
    after stripping. It currently outputs the function addresses, which you can
    look up with `nm` (e.g. `nm -S ./build/example | egrep ' [tT] main\.'`).
  * No garbage collector, yet. Allocated memory is never freed.
  * Many types might not be implemented, but support will probably be pretty
    easy to add by including the correct files from `gofrontend/libgo/runtime`.
  * The scheduler is pretty dumb right now. Passing messages between two
    goroutines is fast (on the Raspberry Pi 3 slightly faster than the standard
    Go compiler!) but any extra goroutine that is started will slow down any
    blocking operation.
  * No support for running multiple goroutines on different threads. I don't
    really have an intention to implement this as it will complicate the
    scheduler and I don't want a complicated scheduler.
  * Most of the standard library doesn't build yet, in particular the `fmt`
    package (which depends on a whole slew of other packages). Some leaf
    packages will work when added to the Makefile manually.
  * No automatic dependency installation, except for a few defined in the
    Makefile.
  * ... probably many more.

## Current status

I mainly developed this as a proof of concept so I'm not sure how I'll continue,
but I'm happy that it works (to some degree). I'm interested in microcontrollers
and really like the Go language so may actually use it in the future, in which
case I surely will improve it.

Also, it depends on gccgo and gccgo seems to be moving to a runtime written in
Go, so I might need to adjust to that in the future.

## Building

Dependencies:

  * An ARM machine, like a Raspberry Pi. Making it work on anything else
    (without backtrace support) should not be very difficult.
  * GCCGO, tested version 6.3.
  * An initialized gofrontend, run `git submodule update --init`.

Then just execute `make PKG=<dirname>` where dirname is the name of the
directory (under `src/`), for example `make PKG=myproject`. You have to put your
source files under `src/`. The executable is then put in the `build/` directory.
You may want to enable link-time optimization with the `LTO=1` flag.

## License

The license is the same BSD 3-clause license as the one used for Go. See the
LICENSE file for details.

## Bare metal support

Requirements for porting this to bare-metal processors (e.g. ARM Cortex-M):

  * Replace all calls to `printf()` with something that logs to a serial
    console.
  * Implement a memory allocator, for example by copying [the one from
    MicroPython](https://github.com/micropython/micropython/blob/master/py/gc.c).
  * Implement context switching, probably using setjmp/longjmp (take a look at
    how MicroPython does it).

## About the 'other' TinyGo

There is another (now dead) project called 'tinygo' [over
here](https://code.google.com/archive/p/tinygo/) and
[here](https://github.com/jackmanlabs/tinygo) that tried to run a very old
variant of the runtime bare-metal. I only discovered it the moment I wanted to
push this project to GitHub.
