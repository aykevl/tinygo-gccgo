// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// This file mostly comes from gofrontend/libgo/runtime/map.goc, but has
// been converted to C.

#include "runtime.h"
#include "map.h"

typedef struct __go_map Hmap;
typedef struct __go_hash_iter hiter;

/* Access a value in a map, returning a value and a presence indicator.  */

bool mapaccess2(MapType *t, Hmap *h, byte *key, byte *val) __asm__("runtime.mapaccess2");
bool mapaccess2(MapType *t, Hmap *h, byte *key, byte *val) {
	byte *mapval;
	size_t valsize;

	mapval = __go_map_index(h, key, 0);
	valsize = t->__val_type->__size;
	if (mapval == nil) {
		__builtin_memset(val, 0, valsize);
		return false;
	} else {
		__builtin_memcpy(val, mapval, valsize);
		return true;
	}
}

/* Delete a key from a map.  */

void mapdelete(Hmap *h, byte *key) __asm__("runtime.mapdelete");
void mapdelete(Hmap *h, byte *key) {
	__go_map_delete(h, key);
}

/* Initialize a range over a map.  */

void mapiterinit(Hmap *h, hiter *it) __asm__("runtime.mapiterinit");
void mapiterinit(Hmap *h, hiter *it) {
	__go_mapiterinit(h, it);
}

/* Move to the next iteration, updating *HITER.  */

void mapiternext(hiter *it) __asm__("runtime.mapiternext");
void mapiternext(hiter *it) {
	__go_mapiternext(it);
}

/* Get the key of the current iteration.  */

void mapiter1(hiter *it, byte *key) __asm__("runtime.mapiter1");
void mapiter1(hiter *it, byte *key) {
	__go_mapiter1(it, key);
}

/* Get the key and value of the current iteration.  */

void mapiter2(hiter *it, byte *key, byte *val) __asm__("runtime.mapiter2");
void mapiter2(hiter *it, byte *key, byte *val) {
	__go_mapiter2(it, key, val);
}

