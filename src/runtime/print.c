
#include "tinygo.h"
#include <inttypes.h>
#include <stdarg.h>

void tinygo_log(const char *msg, int len) {
	while (len--) {
		printf("%c", *msg++); // TODO: there must be a better way
	}
}

void __go_print_nl() {
	tinygo_log("\n", 1);
}

void __go_print_space() {
	tinygo_log(" ", 1);
}

void __go_print_string(const String s) {
	tinygo_log((const char*)s.str, s.len);
}

void __go_print_int64(int64_t n) {
	printf("%" PRId64, n);
}

void __go_print_uint64(uint64_t n) {
	printf("%" PRIu64, n);
}

void runtime_printf(const char *format, ...) {
	// Forward call to standard printf()
	va_list args;
	va_start(args, format);
	vprintf(format, args);
}
