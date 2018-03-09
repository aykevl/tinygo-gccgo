// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The code in this file has been taken from
// gofrontend/libgo/runtime/runtime1.goc and has been converted to C.

#include "runtime.h"
#include "go-type.h"

String typestring(Eface e) __asm__("runtime.typestring");
String typestring(Eface e) {
	return *e.__type_descriptor->__reflection;
}
