
// +build linux

package runtime

import (
	"unsafe"
)

func tinygo_longjmp(*stackframe, uintptr) __asm__("tinygo_longjmp")
func tinygo_setjmp(*stackframe, uintptr) __asm__("tinygo_setjmp")
func tinygo_print_stackitem(r *goroutine, pc_top, pc_call uintptr, sp int) bool __asm__("tinygo_print_stackitem")

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

func Panic(msg interface{}, fatal bool) {
	GR.panicking = &panicElement{msg: msg, next: GR.panicking}

	if !fatal {
		_sp_base := uintptr(unsafe.Pointer(&GR.stack[0]))
		frame := stackframe{}
		tinygo_setjmp(&frame, _sp_base)
		d := GR.d
		for d != nil {
			if err := unwind_frame(&frame); err != "" {
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
		tinygo_fatal()
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
		err := unwind_frame(&childframe)
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
