// Copyright 2011 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// +build !darwin,!dragonfly,!freebsd,!linux,!nacl,!netbsd,!openbsd,!solaris

package time

import (
	"errors"
	"syscall"
)

func readFile(name string) ([]byte, error) {
	return nil, errors.New("unimplemented")
}

func open(name string) (uintptr, error) {
	return 0, errors.New("unimplemented")
}

func closefd(fd uintptr) {
}

func preadn(fd uintptr, buf []byte, off int) error {
	return errors.New("unimplemented")
}

func isNotExist(err error) bool { return err == syscall.ENOENT }
