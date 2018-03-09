// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The code in this file has been taken from
// gofrontend/libgo/runtime/go-iface.goc and has been converted to C.

#include "runtime.h"
#include "go-type.h"
#include "interface.h"

typedef struct __go_type_descriptor descriptor;
typedef struct __go_empty_interface empty_interface;
typedef const struct __go_type_descriptor const_descriptor;

// Compare two type descriptors.
bool ifacetypeeq(const struct __go_type_descriptor *td1, const struct __go_type_descriptor *td2) __asm__("runtime.ifacetypeeq");
bool ifacetypeeq(const struct __go_type_descriptor *td1, const struct __go_type_descriptor *td2) {
	return __go_type_descriptors_equal(td1, td2);
}

// Return the descriptor for an empty interface type.n
const_descriptor* efacetype(empty_interface e) __asm__("runtime.efacetype");
const_descriptor* efacetype(empty_interface e) {
	return e.__type_descriptor;
}

// Return whether we can convert an interface to a type.
bool ifaceI2Tp(descriptor *to, descriptor *from) __asm__("runtime.ifaceI2Tp");
bool ifaceI2Tp(descriptor *to, descriptor *from) {
	return __go_can_convert_to_interface(to, from);
}
