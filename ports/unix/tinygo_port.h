
#pragma once

#define STACK_SIZE (4 * 1024) // 4kB, also see runtime.go
#define TINYGO_HAS_STACKTRACE (1)
#define TINYGO_HAS_GOROUTINE_INFO (1)
#define TINYGO_CAN_RECOVER (1)

// Must be the same as type goroutine in runtime.go.
typedef struct _goroutine_t {
	struct _goroutine_t *next;
	struct _goroutine_t *prev;
	panic_element_t     *panicking;
	size_t     num; // for debugging only
	void       *created_by;
	defer_t    *deferred;
	uintptr_t  canary;
	uintptr_t  stack[STACK_SIZE / sizeof(uintptr_t)]; // TODO: align on page boundary
	ucontext_t context;
} port_goroutine_t;

typedef port_goroutine_t *goroutine_t;

extern port_goroutine_t *goroutine __asm__("runtime.GR");

#define GOROUTINE_GET() (goroutine)
#define GOROUTINE_GET_DEFERRED(g)       (g->deferred)
#define GOROUTINE_SET_DEFERRED(g, val)  (g->deferred = val)
#define GOROUTINE_GET_PANICKING(g)      (g->panicking)
#define GOROUTINE_SET_PANICKING(g, val) (g->panicking = val)
