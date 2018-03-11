
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

typedef struct {
	uintptr_t regs[9]; // r4-r12
	intptr_t  sp;      // r13
	uintptr_t lr;      // r14
	uintptr_t pc;      // r15
} stackframe_t;

__attribute__((naked))
void tinygo_setjmp(stackframe_t *frame, uintptr_t sp_base) {
	__asm__ __volatile__(
		"str r4,  [r0, #0]\n"  // frame.regs[0] = r4
		"str r5,  [r0, #4]\n"  // frame.regs[1] = r5
		"str r6,  [r0, #8]\n"  // frame.regs[2] = r6
		"str r7,  [r0, #12]\n" // frame.regs[3] = r7
		"str r8,  [r0, #16]\n" // frame.regs[4] = r8
		"str r9,  [r0, #20]\n" // frame.regs[5] = r9
		"str r10, [r0, #24]\n" // frame.regs[6] = r10
		"str r11, [r0, #28]\n" // frame.regs[7] = r11
		"str r12, [r0, #32]\n" // frame.regs[8] = r12 -- not necessary?
		"sub r2, sp, r1\n"     // get relative register (for stack[index] operation)
		"lsr r2, r2, #2\n"     // divide by 4, for the same reason
		"str r2,  [r0, #36]\n" // frame.sp = (sp - sp_base) / 4
		"str lr,  [r0, #40]\n" // frame.lr = lr
		"bx  lr\n");           // return
}

__attribute__((naked,noreturn))
void tinygo_longjmp(stackframe_t *frame, uintptr_t sp_absolute) {
	__asm__ __volatile__(
		"ldr r4,  [r0, #0]\n"
		"ldr r5,  [r0, #4]\n"
		"ldr r6,  [r0, #8]\n"
		"ldr r7,  [r0, #12]\n"
		"ldr r8,  [r0, #16]\n"
		"ldr r9,  [r0, #20]\n"
		"ldr r10, [r0, #24]\n"
		"ldr r11, [r0, #28]\n"
		"ldr r12, [r0, #32]\n"
		"mov sp,  r1\n" // sp is passed separatedly
		"ldr r0,  [r0, #40]\n"
		"add r0,  r0, #4\n"
		"bx  r0\n"); // jump to target - ARM recommends we use bx instead of writing to it directly
}
