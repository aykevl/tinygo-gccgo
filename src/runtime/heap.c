
#include "tinygo.h"

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

void * runtime_mallocgc(uintptr_t size, uintptr_t type, uint32_t flags) {
	return tinygo_alloc(size);
}

void * runtime_cnewarray(const Type *typ, intgo n) {
	// TODO: sanity checks?
	return tinygo_alloc(typ->__size * n);
}

void * runtime_cnew(const Type *typ, intgo n) {
	return runtime_cnewarray(typ, 1);
}

uintptr_t runtime_roundupsize(uintptr_t size) {
	return tinygo_roundupsize(size);
}

void __go_register_gc_roots(struct root_list *list) {
	// TODO when we have a real GC
}

void reflect_typedmemmove(const Type *td, void *dst, const void *src) __asm__("reflect.typedmemmove");
void reflect_typedmemmove(const Type *td, void *dst, const void *src) {
	memmove(dst, src, td->__size);
}
