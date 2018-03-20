
#include "src/runtime/tinygo.h"

int main(int argc, char **argv) {
	go_init();
	tinygo_go(go_main, NULL, NULL);
	DEBUG_printf("-- starting with %zu...\n", goroutine->num);
	setcontext(&goroutine->context);
	__builtin_unreachable();
	return 0;
}

static void tinygo_check_canary() {
	if (goroutine->canary != STACK_CANARY) {
		Eface err;
		runtime_newErrorCString("stack overflow", &err);
		runtime_Panic(err, true);
	}
}

__attribute__((noreturn))
void tinygo_goroutine_exit() {
	goroutine_work_counter++;

	DEBUG_printf("-- exit goroutine %zd\n", goroutine->num);

	tinygo_check_canary();

	// cleanup goroutine
	if (goroutine->next == goroutine) {
		// This is the last goroutine.
		// In an MCU (with signals that can start new goroutines) you'd
		// call __WFE() or similar to go in sleep mode.
		DEBUG_printf("-- exit\n");
		tinygo_free(goroutine);
		exit(0);
	}
	goroutine_t r = goroutine;
	r->prev->next = r->next;
	r->next->prev = r->prev;
	goroutine = r->next;

	// WARNING: here we're freeing the stack we're working on!
	//tinygo_free(goroutine); // make things easier for the GC

	// The goroutine data has been freed, so we're not allowed to touch it
	// anymore. However, when we return here r->context.uc_link will still
	// be accessed. So we do the context switch manually.
	DEBUG_printf("-- resuming %zd...\n", goroutine->num);
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

	if (goroutine != NULL) {
		tinygo_check_canary();
	}

	// Allocate and init goroutine
	goroutine_t r = tinygo_alloc(sizeof(port_goroutine_t));
	r->num = ++goroutine_counter;
	r->created_by = created_by;
	r->canary = STACK_CANARY;
	getcontext(&r->context);
	r->context.uc_stack.ss_sp = &r->stack;
	r->context.uc_stack.ss_flags = 0;
	r->context.uc_stack.ss_size = STACK_SIZE;
	r->context.uc_link = NULL;
	makecontext(&r->context, (void(*)())tinygo_run_internal, 2, fn, arg);

	DEBUG_printf("-- create goroutine %zd\n", r->num);

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

void tinygo_block() {
	if (goroutine == goroutine->next) {
		tinygo_deadlocked();
	}
	tinygo_check_canary();
	goroutine_t old = goroutine;
	goroutine_t new = goroutine->next;
	goroutine = new;
	DEBUG_printf("-- switch from %zd to %zd...\n", old->num, new->num);
	swapcontext(&old->context, &new->context);
}

void tinygo_fatal() {
	exit(1);
}

void tinygo_semacquire(volatile uint32_t *addr, bool profile) {
	// This is safe because TinyGo is single-threaded.
	while (*addr == 0) {
		tinygo_block();
	}
	*addr -= 1;
}

void tinygo_semrelease(volatile uint32_t *addr) {
	// This is safe because TinyGo is single-threaded.
	*addr += 1;
}
