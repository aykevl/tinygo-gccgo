
#include "tinygo.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

size_t goroutine_work_counter = 0; // every time work happens, this is incremented

void * tinygo_alloc(size_t size) {
	// This zero-initializes the buffer for easy usage.
	// TODO: allocate on GC heap
	// TODO: check for out-of-memory
	return calloc(1, size);
}

void tinygo_free(void *ptr) {
	free(ptr);
}

// The size that will actually be allocated when doing tinygo_alloc(size).
// Currently it's the same as the requested value, but in the future we
// might have a GC that allocates with fixed block sizes, for example.
size_t tinygo_roundupsize(size_t size) {
	return size;
}

__attribute__((noreturn))
void tinygo_goroutine_exit() {
	goroutine_work_counter++;

	DEBUG_printf("-- exit goroutine %d\n", goroutine->num);

	// cleanup goroutine
	if (goroutine->next == goroutine) {
		// This is the last goroutine.
		// In an MCU (with signals that can start new goroutines) you'd
		// call __WFE() or similar to go in sleep mode.
		DEBUG_printf("-- exit\n");
		tinygo_free(goroutine);
		exit(0);
	}
	goroutine_t *r = goroutine;
	r->prev->next = r->next;
	r->next->prev = r->prev;
	goroutine = r->next;

	// WARNING: here we're freeing the stack we're working on!
	//tinygo_free(goroutine); // make things easier for the GC

	// The goroutine data has been freed, so we're not allowed to touch it
	// anymore. However, when we return here r->context.uc_link will still
	// be accessed. So we do the context switch manually.
	DEBUG_printf("-- resuming %d...\n", goroutine->num);
	setcontext(&goroutine->context);
	__builtin_unreachable();
}

// Helper function for tinygo_go.
// This is the first function called after a goroutine is created. It
// makes sure the goroutine is properly cleaned up after it exits.
void tinygo_run_internal(func fn, void *arg) {
	fn(arg);
	tinygo_goroutine_exit();
}

// Start a new goroutine
void tinygo_go(void *fn, void *arg, void *created_by) {
	static size_t goroutine_counter = 0;

	// Allocate and init goroutine
	goroutine_t *r = tinygo_alloc(sizeof(goroutine_t));
	r->num = ++goroutine_counter;
	r->created_by = created_by;
	r->canary = STACK_CANARY;
	getcontext(&r->context);
	r->context.uc_stack.ss_sp = &r->stack;
	r->context.uc_stack.ss_flags = 0;
	r->context.uc_stack.ss_size = STACK_SIZE;
	r->context.uc_link = NULL;
	makecontext(&r->context, (void(*)())tinygo_run_internal, 2, fn, arg);

	DEBUG_printf("-- create goroutine %d\n", r->num);

	if (goroutine == NULL) {
		goroutine = r;
		r->next = r;
		r->prev = r;
	} else {
		// Add this goroutine to the circular list.
		r->next = goroutine->next;
		r->prev = goroutine;
		r->next->prev = r;
		goroutine->next = r;
	}
}

void * __go_new(void * type, uintptr_t size) {
	if (size == 0) {
		return &runtime_zerobase;
	}
	return tinygo_alloc(size);
}

void * __go_alloc(uintptr_t size) {
	return tinygo_alloc(size);
}

void __go_free(void * ptr) {
	return tinygo_free(ptr);
}

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

void __go_defer(bool *stackframe, func fn, void *arg) {
	DEBUG_printf("deferring function at %p: %p (arg=%p)\n", stackframe, fn, arg);
	defer_t *d = tinygo_alloc(sizeof(defer_t));
	d->stackframe = stackframe;
	d->fn = fn;
	d->arg = arg;
	d->next = goroutine->deferred;
	goroutine->deferred = d;
}

void __go_undefer(bool *stackframe) {
	DEBUG_printf("undeferring functions at frame %p\n", stackframe);
	while (goroutine->deferred != NULL && goroutine->deferred->stackframe == stackframe) {
		defer_t *d = goroutine->deferred;
		d->fn(d->arg);
		goroutine->deferred = d->next;
	}
}

void __go_check_defer(bool *stackframe) {
	runtime_throw("unimplemented: __go_check_defer");
}

void __gccgo_personality_v0() {
	runtime_throw("unimplemented: __gccgo_personality_v0");
}

void * runtime_mallocgc(uintptr_t size, uintptr_t type, uint32_t flags) {
	return tinygo_alloc(size);
}

uint32_t runtime_fastrand1() {
	return 0; // TODO
}

uintptr_t runtime_roundupsize(uintptr_t size) {
	return tinygo_roundupsize(size);
}

void tinygo_block() {
	if (goroutine == goroutine->next) {
		tinygo_deadlocked();
	}
	if (goroutine->canary != STACK_CANARY) {
		Eface err;
		runtime_newErrorCString("stack overflow", &err);
		runtime_Panic(err, true);
	}
	goroutine_t *old = goroutine;
	goroutine_t *new = goroutine->next;
	goroutine = new;
	DEBUG_printf("-- switch from %d to %d...\n", old->num, new->num);
	swapcontext(&old->context, &new->context);
}
