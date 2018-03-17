
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

void * __go_new(void *type, uintptr_t size) {
	if (size == 0) {
		return &runtime_zerobase;
	}
	return tinygo_alloc(size);
}

void * __go_new_nopointers(void *type, uintptr_t size) {
	return __go_new(type, size);
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

void __go_register_gc_roots(struct root_list *list) {
	// TODO when we have a real GC
}
