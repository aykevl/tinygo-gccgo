
package runtime

import (
	"unsafe"
)

const STACK_SIZE = 2 * 1024
const STACK_SIZE_PTRS = STACK_SIZE / unsafe.Sizeof(uintptr)

// Must be the same as defer_t in tinygo.h.
type deferred struct {
	fn         unsafe.Pointer
	arg        unsafe.Pointer
	stackframe *bool
	next       *deferred
}

// Must be the same as goroutine_t in tinygo.h.
type goroutine struct {
	next       *goroutine
	prev       *goroutine
	num        uintptr
	created_by unsafe.Pointer
	d          *deferred
	canary     uintptr
	stack      [STACK_SIZE_PTRS]uintptr
	//context    ucontext_t; // TODO - this is at the end so it doesn't really matter
}

// Current goroutine.
// Referenced from C code (as runtime.GR).
var GR *goroutine

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
func tinygo_get_lr_sp() (uintptr, uintptr) __asm__("tinygo_get_lr_sp")
func tinygo_print_stackitem(r *goroutine, pc_top, pc_call uintptr, sp int) bool __asm__("tinygo_print_stackitem")
func tinygo_get_code_bounds() (uintptr, uintptr) __asm__("tinygo_get_code_bounds")
func tinygo_get_unwind_sections() (uintptr, uintptr) __asm__("tinygo_get_unwind_sections")
func exit(int) __asm__("exit")

func Panic(msg interface{}, fatal bool) {
	if !fatal {
		// TODO: actually unwind the stack and call __go_undefer where
		// appropriate.
		d := GR.d
		for d != nil {
			tinygo_call_fn(d.fn, d.arg)
			d = d.next
		}
		print("panic: ")
	}
	Printany(msg)
	println()

	println("\ngoroutine", GR.num, "[running]:")

	unwind(GR)

	if fatal {
		exit(1)
	} else {
		tinygo_goroutine_exit()
	}
}
