
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

// One call to panic(), stacked in the goroutine.
type panicElement struct {
	msg     interface{}
	retaddr uintptr
	next    *panicElement
}

// Must be the same as goroutine_t in tinygo.h.
type goroutine struct {
	next       *goroutine
	prev       *goroutine
	panicking  *panicElement
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
func tinygo_print_stackitem(r *goroutine, pc_top, pc_call uintptr, sp int) bool __asm__("tinygo_print_stackitem")
func tinygo_get_code_bounds() (uintptr, uintptr) __asm__("tinygo_get_code_bounds")
func tinygo_get_unwind_sections() (uintptr, uintptr) __asm__("tinygo_get_unwind_sections")
func tinygo_longjmp(*stackframe, uintptr) __asm__("tinygo_longjmp")
func tinygo_setjmp(*stackframe, uintptr) __asm__("tinygo_setjmp")
func exit(int) __asm__("exit")

func Panic(msg interface{}, fatal bool) {
	GR.panicking = &panicElement{msg: msg, next: GR.panicking}

	if !fatal {
		// TODO: actually unwind the stack and call __go_undefer where
		// appropriate.
		_sp_base := uintptr(unsafe.Pointer(&GR.stack[0]))
		frame := stackframe{}
		tinygo_setjmp(&frame, _sp_base)
		d := GR.d
		for d != nil {
			if err := unwind_frame(GR, &frame); err != "" {
				break // unwind error, will be printed later in the stack trace
			}
			for d != nil && uintptr(unsafe.Pointer(d.stackframe)) <= uintptr(unsafe.Pointer(&GR.stack[frame.sp])) {
				// TODO: some deferred functions are not called
				tinygo_call_fn(d.fn, d.arg)
				d = d.next
			}
			if GR.panicking == nil {
				tinygo_longjmp(&frame, uintptr(unsafe.Pointer(&GR.stack[frame.sp])))
				// unreachable
			}
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

func unwind(r *goroutine) {
	_sp_base := uintptr(unsafe.Pointer(&GR.stack[0]))
	frame := stackframe{}
	tinygo_setjmp(&frame, _sp_base)

	childframe := frame
	parentframe := childframe
	for {
		parentframe = childframe // move up a stack frame
		err := unwind_frame(r, &childframe)
		if err != "" {
			// SP or PC was out of range, probably an error while walking
			// the stack.
			tinygo_unwind_fail(childframe.pc, err)
			break
		}
		if tinygo_print_stackitem(r, childframe.pc + 4, parentframe.pc + 4, childframe.sp) {
			// We're done: this is the last frame.
			break
		}
	}
}
