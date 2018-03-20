

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

#if TINYGO_HAS_STACKTRACE
bool tinygo_print_stackitem(goroutine_t r, uint32_t *pc_top, uint32_t *pc_call, intptr_t sp) {
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
#endif

void tinygo_unwind_fail(void *pc, String msg) {
	printf("[cannot unwind further: %.*s, PC=%p]\n", (int)msg.len, msg.str, pc);
}

void tinygo_unwind(goroutine_t r, uint32_t *pc, uint32_t *sp) {
	// The current goroutine is by definition running, so hard-code the
	// status [running].
	#if TINYGO_HAS_GOROUTINE_INFO
	printf("\ngoroutine %zu [running]:\n", r->num);
	#else
	printf("\ngoroutine [running]:\n");
	#endif

	tinygo_port_unwind(r, pc, sp);
}

void __go_panic(struct __go_empty_interface msg) {
	runtime_Panic(msg, false);
}

struct __go_empty_interface __go_recover() {
	#if !TINYGO_CAN_RECOVER
	runtime_throw("unimplemented: recover()");
	#else
	goroutine_t g = GOROUTINE_GET();
	panic_element_t *top = GOROUTINE_GET_PANICKING(g);
	if (top == NULL) {
		struct __go_empty_interface ret = {NULL, NULL};
		return ret;
	}
	GOROUTINE_SET_PANICKING(g, top->next);
	return top->msg;
	#endif
}

bool __go_can_recover(void *retaddr) {
	goroutine_t g = GOROUTINE_GET();
	defer_t *deferred = GOROUTINE_GET_DEFERRED(g);
	if (deferred == NULL) {
		return false;
	}
	return deferred->retaddr == retaddr;
}

bool __go_set_defer_retaddr(void *retaddr) {
	goroutine_t g = GOROUTINE_GET();
	defer_t *deferred = GOROUTINE_GET_DEFERRED(g);
	if (deferred != NULL) {
		deferred->retaddr = retaddr;
	}
	return 0;
}

void __go_defer(bool *stackframe, func fn, void *arg) {
	DEBUG_printf("deferring function at %p: %p (arg=%p)\n", stackframe, fn, arg);
	defer_t *d = tinygo_alloc(sizeof(defer_t));
	d->stackframe = stackframe;
	d->fn = fn;
	d->arg = arg;
	goroutine_t g = GOROUTINE_GET();
	d->next = GOROUTINE_GET_DEFERRED(g);
	GOROUTINE_SET_DEFERRED(g, d);
}

void __go_undefer(bool *stackframe) {
	DEBUG_printf("undeferring functions at frame %p\n", stackframe);
	goroutine_t g = GOROUTINE_GET();
	defer_t *d = GOROUTINE_GET_DEFERRED(g);
	while (d != NULL && d->stackframe == stackframe) {
		d->fn(d->arg);
		d = d->next;
		GOROUTINE_SET_DEFERRED(g, d);
	}
}

void __go_check_defer(bool *stackframe) {
	runtime_throw("unimplemented: __go_check_defer");
}
