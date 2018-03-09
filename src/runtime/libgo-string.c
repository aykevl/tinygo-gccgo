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
