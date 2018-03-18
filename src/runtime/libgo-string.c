// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The code in this file has been taken from
// gofrontend/libgo/runtime/string.goc and has been converted to C.

#include "runtime.h"
#include "go-string.h"

#define charntorune(pv, str, len) __go_get_rune(str, len, pv)

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
		// retk=0 is end of iteration
		return 0;
	}

	int32 l = s.str[k];
	if (l < 0x80) {
		return k + 1;
	}

	// multi-char rune
	return k + charntorune(&l, s.str+k, s.len-k);
}

struct stringiter2 { int retk; int32 retv; };
struct stringiter2 stringiter2(String s, int k) __asm__("runtime.stringiter2");
struct stringiter2 stringiter2(String s, int k) {
	struct stringiter2 r;
	if (k >= s.len) {
		// retk=0 is end of iteration
		r.retk = 0;
		r.retv = 0;
		return r;
	}

	r.retv = s.str[k];
	if (r.retv < 0x80) {
		r.retk = k + 1;
		return r;
	}

	// multi-char rune
	r.retk = k + charntorune(&r.retv, s.str+k, s.len-k);
	return r;
}
