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
typedef struct __go_interface interface;
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

// Return the descriptor for a non-empty interface type.
const_descriptor * ifacetype(interface i) __asm__("runtime.ifacetype");
const_descriptor * ifacetype(interface i) {
	if (i.__methods == NULL) {
		return NULL;
	}
	return i.__methods[0];
}

// Convert an empty interface to an empty interface.
struct ifaceE2E2 { empty_interface ret; bool ok; };
struct ifaceE2E2 ifaceE2E2(empty_interface e) __asm__("runtime.ifaceE2E2");
struct ifaceE2E2 ifaceE2E2(empty_interface e) {
	struct ifaceE2E2 r;
	r.ret = e;
	r.ok = e.__type_descriptor != NULL;
	return r;
}

// Convert a non-empty interface to an empty interface.
struct ifaceI2E2 { empty_interface ret; bool ok; };
struct ifaceI2E2 ifaceI2E2(interface i) __asm__("runtime.ifaceI2E2");
struct ifaceI2E2 ifaceI2E2(interface i) {
	struct ifaceI2E2 r;
	if (i.__methods == NULL) {
		r.ret.__type_descriptor = NULL;
		r.ret.__object = NULL;
		r.ok = 0;
	} else {
		r.ret.__type_descriptor = i.__methods[0];
		r.ret.__object = i.__object;
		r.ok = 1;
	}
	return r;
}


// Convert an empty interface to a non-empty interface.
struct ifaceE2I2 { interface ret; bool ok; };
struct ifaceE2I2 ifaceE2I2(descriptor *inter, empty_interface e) __asm__("runtime.ifaceE2I2");
struct ifaceE2I2 ifaceE2I2(descriptor *inter, empty_interface e) {
	struct ifaceE2I2 r;
	if (e.__type_descriptor == NULL) {
		r.ret.__methods = NULL;
		r.ret.__object = NULL;
		r.ok = 0;
	} else {
		r.ret.__methods = __go_convert_interface_2(inter,
		                                           e.__type_descriptor,
		                                           1);
		r.ret.__object = e.__object;
		r.ok = r.ret.__methods != NULL;
	}
	return r;
}

// Convert a non-empty interface to a non-empty interface.
struct ifaceI2I2 { interface ret; bool ok; };
struct ifaceI2I2 ifaceI2I2(descriptor *inter, interface i) __asm__("runtime.ifaceI2I2");
struct ifaceI2I2 ifaceI2I2(descriptor *inter, interface i) {
	struct ifaceI2I2 r;
	if (i.__methods == NULL) {
		r.ret.__methods = NULL;
		r.ret.__object = NULL;
		r.ok = 0;
	} else {
		r.ret.__methods = __go_convert_interface_2(inter, i.__methods[0], 1);
		r.ret.__object = i.__object;
		r.ok = r.ret.__methods != NULL;
	}
	return r;
}

// Convert an empty interface to a pointer type.
struct ifaceE2T2P { void *ret; bool ok; };
struct ifaceE2T2P ifaceE2T2P(descriptor *inter, empty_interface e) __asm__("runtime.ifaceE2T2P");
struct ifaceE2T2P ifaceE2T2P(descriptor *inter, empty_interface e) {
	struct ifaceE2T2P r;
	if (!__go_type_descriptors_equal(inter, e.__type_descriptor)) {
		r.ret = NULL;
		r.ok = 0;
	} else {
		r.ret = e.__object;
		r.ok = 1;
	}
	return r;
}

// Convert a non-empty interface to a pointer type.
struct ifaceI2T2P { void *ret; bool ok; };
struct ifaceI2T2P ifaceI2T2P(descriptor *inter, interface i) __asm__("runtime.ifaceI2T2P");
struct ifaceI2T2P ifaceI2T2P(descriptor *inter, interface i) {
	struct ifaceI2T2P r;
	if (i.__methods == NULL || !__go_type_descriptors_equal(inter, i.__methods[0])) {
		r.ret = NULL;
		r.ok = 0;
	} else {
		r.ret = i.__object;
		r.ok = 1;
	}
	return r;
}

// Convert an empty interface to a non-pointer type.
bool ifaceE2T2(descriptor *inter, empty_interface e, void *ret) __asm__("runtime.ifaceE2T2");
bool ifaceE2T2(descriptor *inter, empty_interface e, void *ret) {
	if (!__go_type_descriptors_equal(inter, e.__type_descriptor)) {
		memset(ret, 0, inter->__size);
		return false;
	} else {
		memcpy(ret, e.__object, inter->__size);
		return true;
	}
}

// Convert a non-empty interface to a non-pointer type.
bool ifaceI2T2(descriptor *inter, interface i, void *ret) __asm__("runtime.ifaceI2T2");
bool ifaceI2T2(descriptor *inter, interface i, void *ret) {
	if (i.__methods == NULL || !__go_type_descriptors_equal(inter, i.__methods[0])) {
		memset(ret, 0, inter->__size);
		return false;
	} else {
		memcpy(ret, i.__object, inter->__size);
		return true;
	}
}

// Return whether we can convert an interface to a type.
bool ifaceI2Tp(descriptor *to, descriptor *from) __asm__("runtime.ifaceI2Tp");
bool ifaceI2Tp(descriptor *to, descriptor *from) {
	return __go_can_convert_to_interface(to, from);
}
