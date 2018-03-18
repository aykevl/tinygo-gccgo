// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The code in this file has been taken from
// gofrontend/libgo/runtime/reflect.goc and has been converted to C.

#include "runtime.h"
#include "go-type.h"
#include "interface.h"
#include "go-panic.h"

void ifaceE2I(Type *inter, Eface e, Iface *ret) __asm__("reflect.ifaceE2I");
void ifaceE2I(Type *inter, Eface e, Iface *ret) {
	const Type *t;
	Eface err;

	t = e.__type_descriptor;
	if (t == NULL) {
		// explicit conversions require non-nil interface value.
		runtime_newTypeAssertionError(
			nil, nil, inter->__reflection,
			nil, &err);
		runtime_panic(err);
	}
	ret->__object = e.__object;
	ret->__methods = __go_convert_interface(inter, t);
}

