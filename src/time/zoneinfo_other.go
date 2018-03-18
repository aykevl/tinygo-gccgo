// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// +build !darwin,!dragonfly,!freebsd,!linux,!nacl,!netbsd,!openbsd,!plan9,!solaris,!windows

// Parse "zoneinfo" time zone file.
// This is a fairly standard file format used on OS X, Linux, BSD, Sun, and others.
// See tzfile(5), http://en.wikipedia.org/wiki/Zoneinfo,
// and ftp://munnari.oz.au/pub/oldtz/

package time

import (
	"errors"
)

func initLocal() {
	// Fall back to UTC.
	localLoc.name = "UTC"
}

func loadLocation(name string) (*Location, error) {
	return nil, errors.New("unknown time zone " + name)
}
