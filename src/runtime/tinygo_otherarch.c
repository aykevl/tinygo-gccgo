
#include "tinygo.h"

// Dummy implementations to satisfy the linker, won't actually be used.

typedef struct {
	intptr_t  sp;
	uintptr_t pc;
} stackframe_t;

void tinygo_setjmp(stackframe_t *frame, uintptr_t sp_base) {
}

void tinygo_longjmp(stackframe_t *frame, uintptr_t sp_absolute) {
	printf("TODO: longjmp\n");
	tinygo_fatal();
}
