
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
