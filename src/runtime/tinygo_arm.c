
#include "tinygo.h"

// .ARM.exidx start and end addresses.
extern uint32_t __exidx_start[];
extern uint32_t __exidx_end[];

// Find the start and end addresses of executable code - to verify valid
// pointers.
// https://stackoverflow.com/questions/7370407/get-the-start-and-end-address-of-text-section-in-an-executable
extern uint32_t __executable_start[];
extern uint32_t __etext[];

// The end address of .text.
extern uint32_t __etext[];

void tinygo_get_unwind_sections(uint32_t *args[]) {
	args[0] = __exidx_start;
	args[1] = __exidx_end;
}

void tinygo_get_code_bounds(uint32_t *args[]) {
	args[0] = __executable_start;
	args[1] = __etext;
}

__attribute__((naked))
void tinygo_get_lr_sp(uintptr_t *args) {
	// Capture the current stack pointer and program counter of the
	// caller. The exact location of the PC shouldn't matter. The stack
	// pointer shouldn't matter too much either, as long as it's at the
	// same position captured as the program counter.
	__asm__ __volatile__(
		"str lr, [r0, #0]\n"
		"str sp, [r0, #4]\n"
		"bx lr\n");
}
