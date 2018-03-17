
package runtime

import (
	"unsafe"
)

const GOARCH = "arm"

// Choose which undwinder implementation to use. "armdecode" is the one that
// actually works at the moment.
const UNWINDER = "armdecode"
//const UNWINDER = "exidx"

type stackframe struct {
	regs[9] uintptr // r4-r12
	sp int          // r13
	lr uintptr      // r14
	pc uintptr      // r15
}

func unwind_frame(frame *stackframe) string {
	frame.pc = frame.lr
	frame.lr = 0
	if UNWINDER == "armdecode" {
		return unwind_armdecode(GR, frame)
	} else if UNWINDER == "exidx" {
		return unwind_exidx(GR, frame)
	} else {
		panic("unreachable")
	}
}

func unwind_armdecode(r *goroutine, f *stackframe) string {
	stext, etext := tinygo_get_code_bounds()
	//pc_call := f.pc
	for f.sp >= 0 && f.sp < len(r.stack) && f.pc >= stext && f.pc < etext {
		f.pc -= 4
		instr := *(*uint32)(unsafe.Pointer(f.pc))
		//printf("unwinding... PC=%p SP=%p LR=%p: ", pc, f.sp, lr);
		if ((instr >> 16) & 0xffff) == 0xe92d { // STMFD sp!, {...}: P=1, U=0, S=0, W=1, L=0
			// STM is: 1110 100P USWL Rn reglist...
			//        cond
			// Note: condition code 1110 means "always"
			//printf("STMFD sp!, {...}\n"); // aka push
			for regnr := uint(0); regnr < 16; regnr++ {
				if (instr >> regnr) & 1 == 1 {
					//printf("\tr%-2d = %x\n", regnr, *sp);
					if regnr == 14 { // lr
						f.lr = r.stack[f.sp] - 4
						// We have found the link register.
						// Note that we will continue walking up, as there
						// might be more SP-altering instructions (but
						// we'll break at the first unrecognized/non-SP-
						// related function as that means we're past the
						// prologue).
					} else if regnr == 13 {
						// TODO sp
					} else if regnr >= 4 && regnr < 13 { // reg 4..12: callee-saved registers
						f.regs[regnr-4] = r.stack[f.sp]
					}
					// reading back so reversing direction
					f.sp += 1
				}
			}
		} else if (((instr >> 12) & 0xffeff) == 0xe24dd) { // SUB sp, sp, #imm
			imm := instr & 0xff
			shift := (instr >> 8) & 0xf
			//printf("SUB sp, sp, #%d\n", imm << shift);
			f.sp += int(imm << shift) / 4 // reverse operation, so ADD instead of SUB
		} else {
			//printf("unknown instruction\n");
			if f.lr != 0 {
				// Done with this function. Let's move up a stack frame!
				return "" // OK
			}
		}
	}

	return "pc or sp out of range"
}

type exidxEntry struct {
	address uintptr
	command uintptr
}

func unwind_exidx(r *goroutine, f *stackframe) string {
	// Scan trough the unwind table until we've found the right PC.
	// https://wiki.linaro.org/KenWerner/Sandbox/libunwind?action=AttachFile&do=get&target=libunwind-LDS.pdf
	// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0038b/IHI0038B_ehabi.pdf

	// Load .ARM.exidx from __exidx_start and __exidx_end and create a slice
	// with exactly the contents of it.
	// https://github.com/golang/go/wiki/cgo#turning-c-arrays-into-go-slices
	stext, etext := tinygo_get_code_bounds()
	exidx_start, exidx_end := tinygo_get_unwind_sections()
	size := (exidx_end - exidx_start) / 4
	exidx := (*[1 << 30]exidxEntry)(unsafe.Pointer(exidx_start))[:size/2:size/2]
	_ = exidx

	if !(f.sp >= 0 && f.sp < len(r.stack) && f.pc >= stext && f.pc < etext) {
		return "pc or sp out of range"
	}
	entry := exidxEntry{}
	functionAddress := uintptr(0)
	entryNum := -1

	// Do a linear scan for the right function.
	// We could improve here with a binary search when performance becomes
	// an issue, but a linear scan is easier to implement right now.
	//println("\nsearching for address:", hex(f.pc - 4))
	for i, e := range exidx {
		addr := e.address + exidx_start + uintptr(i) * 8 - (1 << 31)
		//println("  found address + command:", i, hex(addr), hex(e.command))
		if addr > f.pc - 4 {
			//println("\tcheck")
			break
		}
		entry = e
		functionAddress = addr
		entryNum = i
	}
	//println("using entry for address:", hex(functionAddress), "raw:", hex(entry.address), hex(entry.command))

	if entry.command == 1 {
		return "not in unwind table"
	} else if entry.command >> 31 == 1 {
		//println("function encoded inline:", hex(entry.command))
		// Obtain unwind instructions
		instructions := []byte{
			byte(entry.command >> 16),
			byte(entry.command >> 8),
			byte(entry.command)}
		_ = instructions
		if f.decode(r, instructions) {
			return "unknown unwind instruction"
		}
	} else {
		// Pointer to the exidx table.
		_ = entryNum
		//exidxAddress := entry.command + exidx_start + uintptr(entryNum) * 8 - (1 << 31) + 4
		//println("function encoded externally:", hex(exidxAddress), hex(entry.command), entryNum)

		// Pointer to the personality function.
		//personalityAddressRelative := *(*uintptr)(unsafe.Pointer(exidxAddress))
		//personalityAddress := exidxAddress - ((1 << 31) - personalityAddressRelative)
		//println("personality function:", hex(personalityAddressRelative), hex(personalityAddress))

		// Now we're supposed to call the personality function, I think.

		// Construct a byteslice from the pointer in `address`.
		// TODO: length of slice
		//instructions := (*[1 << 30]byte)(unsafe.Pointer(exidxAddress + 4))[:256:256]
		//if f.decode(r, instructions) {
		//	break
		//}
		return "external unwind instructions"
	}

	f.pc = functionAddress - 4
	f.lr -= 4

	return ""
}

// Decode takes a stream of unwind instructions (possibly unbounded) and decodes
// them until the first finish or unrecognized instruction. It returns true on
// failure.
func (f *stackframe) decode(r *goroutine, instructions []byte) bool {
	// See section 9.3, "Frame unwinding instructions":
	// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0038b/IHI0038B_ehabi.pdf

	for i := 0; i < len(instructions); i++ {
		instr := instructions[i]
		// Interpret stack unwinding instructions.
		// This is something quite different from ARM instructions.
		//println("   ", hex(uintptr(instr)))
		switch {
		case instr & 0xc0 == 0x00: // 00xxxxxx
			imm := int(instr & 0x3f)
			f.sp = f.sp + imm + 1
			//println("\tsp +=", (imm << 2) + 4)
		case instr & 0xf8 == 0xa8: // 10101nnn
			imm := int(instr & 0x07)
			// pop r4-r[4+nnn], r14
			f.sp += imm * 1 + 1
			f.lr = r.stack[f.sp]
			//println("\tpop PC:", hex(f.lr), "registers:", imm + 1)
		case instr == 0xb0: // 10110000: finish
			break
		case instr & 0xf0 == 0x80:
			i++
			regs := (uintptr(instr & 0x0f) << 8) | uintptr(instructions[i])
			for regnr := uint(15); regnr >= 4; regnr-- {
				if (regs >> (regnr - 4)) & 1 == 1 {
					//println("\t\tpop reg:", regnr)
					f.sp++
					if regnr == 14 { // lr
						f.lr = r.stack[f.sp]
					}
					if regnr == 13 { // pc
						return true // TODO
					}
				}
			}
		default:
			//println("\tunknown unwind instruction:", hex(uintptr(instr)))
			return true
		}
	}

	// implicit finish instruction
	f.pc = f.lr
	return false
}
