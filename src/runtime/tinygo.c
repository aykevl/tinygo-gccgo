
#include "tinygo.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

size_t goroutine_work_counter = 0; // every time work happens, this is incremented

// 'go' keyword
G* __go_go(func fn, void *arg) {
	uint32_t *created_by = 0;
#if defined(__arm__)
	__asm__("mov %[created_by], lr\n" : [created_by] "=r" (created_by));
	created_by -= 1; // adjust PC offset
#endif
	tinygo_go(fn, arg, created_by);
	return NULL; // TODO?
}

void tinygo_call_fn(func fn, void *arg) {
	fn(arg);
}

void __gccgo_personality_v0() {
	runtime_throw("unimplemented: __gccgo_personality_v0");
}

uint32_t runtime_fastrand1() {
	return 0; // TODO
}

bool sync_runtime_canSpin(int i) __asm__("sync.runtime_canSpin");
bool sync_runtime_canSpin(int i) {
	return false;
}

void sync_runtime_doSpin() __asm__("sync.runtime_doSpin");
void sync_runtime_doSpin() {
}

void sync_runtime_Syncsemcheck() __asm__("sync.runtime_Syncsemcheck");
void sync_runtime_Syncsemcheck() {
	// this is only a sanity check, skip it
}

void sync_runtime_registerPoolCleanup(FuncVal *f) __asm__("sync.runtime_registerPoolCleanup");
void sync_runtime_registerPoolCleanup(FuncVal *f) {
	// not necessary?
}

bool math_hasSSE4() __asm__("math.hasSSE4");
bool math_hasSSE4() {
	return false;
}

__attribute__((weak))
void syscall_init() __asm__("syscall..import");
void syscall_init() {
	// syscall doesn't seem to need initialization?
}
