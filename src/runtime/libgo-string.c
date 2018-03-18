// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The code in this file has been taken from
// gofrontend/libgo/runtime/string.goc and has been converted to C.

#include "runtime.h"

intgo runtime_findnull(const byte *s) {
	if (s == NULL) {
		return 0;
	}
	return __builtin_strlen((const char*) s);
}

String runtime_gostringnocopy(const byte *str) __asm__("runtime.cstringToGo");
String runtime_gostringnocopy(const byte *str)
{
	String s;

	s.str = str;
	s.len = runtime_findnull(str);
	return s;
}

int runtime_stringiter(String s, int k) __asm__("runtime.stringiter");
int runtime_stringiter(String s, int k) {
	if (k >= s.len) {
		return 0;
	}

	if (s.str[k] < 0x80) {
		return k + 1;
	}

	runtime_throw("todo: stringiter");
}
