

#include "tinygo.h"

__attribute__((noreturn))
void runtime_panicstring(const char *s) {
	printf("runtime panic: %s\n", s);
	tinygo_goroutine_exit();
}

void runtime_throw(const char *s) {
	printf("fatal error: %s\n", s);
	exit(1);
}

void tinygo_deadlocked() {
	runtime_throw("deadlock");
}

bool tinygo_print_stackitem(goroutine_t *r, uint32_t *pc_top, uint32_t *pc_call, intptr_t sp) {
	bool found_root = false;
	if (pc_top == (uint32_t*)__go_panic) {
		printf("panic(err)");
	} else if (pc_top == (uint32_t*)tinygo_run_internal) {
		if (r->created_by != NULL) {
			printf("created by ??? (%p)\n", r->created_by);
		} else {
			printf("created by ???\n");
		}
		return true;
	} else if (pc_top == (uint32_t*)go_main) {
		printf("main.main()");
		found_root = true;
	} else if (pc_top == (uint32_t*)runtime_Panic) {
		printf("runtime.Panic(...)");
	} else {
		// TODO: figure out function name
		printf("\?\?\?(...)");
	}
	printf(" at %p\n", pc_top);
	printf("\t%p (+0x%tx, stack usage: %ld/%u)\n", pc_call - 1, ((pc_call - pc_top - 1) * sizeof(func)), (long)(STACK_SIZE-sp*sizeof(uintptr_t)), STACK_SIZE);
	return found_root;
}

void tinygo_unwind_fail(void *pc, String msg) {
	printf("[cannot unwind further: %.*s, PC=%p]\n", (int)msg.len, msg.str, pc);
}

void tinygo_unwind(goroutine_t *r, uint32_t *pc, uint32_t *sp) {
	// The current goroutine is by definition running, so hard-code the
	// status [running].
	printf("\ngoroutine %zu [running]:\n", r->num);

	tinygo_port_unwind(r, pc, sp);
}

void __go_panic(struct __go_empty_interface msg) {
	runtime_Panic(msg, false);
}
