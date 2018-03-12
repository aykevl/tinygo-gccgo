
#include "tinygo.h"

int main(int argc, char **argv) {
	tinygo_go(go_main, NULL, NULL);
	DEBUG_printf("-- starting with %u...\n", goroutine->num);
	setcontext(&goroutine->context);
	__builtin_unreachable();
	return 0;
}
