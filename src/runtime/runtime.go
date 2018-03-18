
package runtime

import (
	"unsafe"
)

const STACK_SIZE = 2 * 1024
const STACK_SIZE_PTRS = STACK_SIZE / unsafe.Sizeof(uintptr)

// One call to panic(), stacked in the goroutine.
type panicElement struct {
	msg     interface{}
	retaddr uintptr
	next    *panicElement
}

// Simple helper function to turn a number into a hex string.
func hex(i uintptr) string {
	s := ""
	if i == 0 {
		return "0x0"
	}
	for ; i != 0; i >>= 4 {
		s = string("0123456789abcdef"[i & 0xf]) + s
	}
	return "0x" + s
}

// Various declarations for functions defined in C sources (or libc).
func tinygo_goroutine_exit() __asm__("tinygo_goroutine_exit")
func tinygo_unwind_fail(uintptr, string) __asm__("tinygo_unwind_fail")
func tinygo_call_fn(unsafe.Pointer, unsafe.Pointer) __asm__("tinygo_call_fn")
func tinygo_get_code_bounds() (uintptr, uintptr) __asm__("tinygo_get_code_bounds")
func tinygo_get_unwind_sections() (uintptr, uintptr) __asm__("tinygo_get_unwind_sections")
func tinygo_fatal() __asm__("tinygo_fatal")

func GOMAXPROCS(n int) int {
	return 1
}

type Func struct {
}

func FuncForPC(pc uintptr) *Func {
	return nil; // TODO
}

func (f *Func) Name() string {
	return "" // unreachable
}

func Caller(skip int) (pc uintptr, file string, line int, ok bool) {
	return 0, "", 0, false; // TODO
}
