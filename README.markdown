# TinyGo GCC (abandoned)

**This project is abandoned**. See [Current status](#current-status) below.

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
$ tinygo build -lto -o hello ./src/hello/

$ strip hello

$ ./hello
hello, world!

$ /bin/ls -lh hello
-rwxrwxr-x 1 ayke ayke 5,5K mrt  9 23:43 build/hello

$ size hello
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

  * Works with gccgo 6.3 (Debian stretch) and 5.2 (xtensa-esp32-elf), no other
    compiler versions have been tested.
  * `recover()` has bugs (but seems to work somewhat).
  * No function names or line numbers in the `panic()` output, and no backtraces
    on anything except ARM. I would like to fix this at some time, but it will
    increase the binary size if it must work after stripping. It currently
    outputs the function addresses, which you can look up with `nm` (e.g. `nm -S
    ./build/example | egrep ' [tT] main\.'`).
  * No garbage collector, yet. Allocated memory is never freed.
  * Many types might not be implemented, but support will probably be pretty
    easy to add by including the correct files from `gcc/libgo/runtime`.
  * The scheduler is pretty dumb right now. Passing messages between two
    goroutines is fast (on the Raspberry Pi 3 slightly faster than the standard
    Go compiler!) but any extra goroutine that is started will slow down any
    blocking operation.
  * No support for running multiple goroutines on different threads. I don't
    really have an intention to implement this as it will complicate the
    scheduler and I don't want a complicated scheduler.
  * Most of the standard library doesn't build yet, in particular the `fmt`
    package (which depends on a whole slew of other packages).
  * ... probably many more.

## Current status

At the moment I'm **no longer working on this project**. The main reason is that
it turns out that the code `gccgo` produces is relatively bloated and is written
specifically for the regular Go runtime, which is optimized for speed and not
RAM/flash usage. I'm now working on
[a new implementation based on LLVM](https://github.com/aykevl/tinygo) just like
[llgo](https://github.com/llvm-mirror/llgo).

This was mostly a proof of concept and I don't expect to be working on it in the
future, but I'm happy that it works (to some degree).

Also, it depends on gccgo and gccgo seems to be moving to a runtime written in
Go, so I might need to adjust to that in the future.

## Building

Dependencies:

  * Installed `gcc` and `gccgo`, tested version 6.3.
  * Installed `go` tool.

Build steps:

  * Fetch GCC sources (needed to build the runtime). Do this by running:

        $ git submodule update --init

  * Build the runtime. For example, for the `unix` target:

        $ cd ports/unix
        $ make -j8

  * Add the `tinygo` script to your `$PATH`, for example by adding this project
    directory to your `$PATH`.

Now you can use `tinygo` just like you're used from the `go` tool, except that
it is still very limited in which packages it can build.

## License

The license is the same BSD 3-clause license as the one used for Go. See the
LICENSE file for details.

Some code has a different (but compatible) license. In particular, this project
contains some code from the [MicroPython
project](https://github.com/micropython/micropython) which is licensed under the
MIT license.

## Bare metal support

### ESP32

There is initial (very limited) support for the
[ESP32](https://en.wikipedia.org/wiki/ESP32). A blinky example:

```go
package main

import (
    "machine"
    "time"
)

func main() {
    led := machine.LED_BUILTIN
    led.Configure(machine.GPIOConfig{Mode: machine.GPIO_OUTPUT})
    high := false
    for {
        high = !high
        led.Set(high)
        time.Sleep(200 * time.Millisecond)
    }
}
```

Implemented so far:

  * GPIO peripheral (only pin 2 and 4, others can be added easily)
  * Goroutines
  * Channels
  * Sleep
  * Stack overflow checking

## Other (similar) projects

  * [libmigo](https://github.com/nutterts/libmigo): implements a bare-bones
    libgo for an x86 kernel.
  * [gopher-os](https://github.com/achilleasa/gopher-os]) appears to be still
    alive. Also see the
    [Golang UK Conference](https://www.youtube.com/watch?v=8T3VxGrrJwc) about it.
  * There is another (now dead) project called 'tinygo'
    [over here](https://code.google.com/archive/p/tinygo/) and
    [here](https://github.com/jackmanlabs/tinygo) that tried to run a very old
    variant of the runtime bare-metal. I only discovered it the moment I wanted
    to push this project to GitHub.
  * [gofy](http://gofy.cat-v.org/) is yet another kernel.
    [Appears to be dead](https://groups.google.com/forum/#!topic/gofy/nVB1IHkc5Lg).

Note that most of these are meant for kernel development. Kernel development,
although nice, is not the goal of TinyGo. Instead, the runtime should be kept as
small as possible to eventually make it work on microcontrollers with much more
limited resources.
