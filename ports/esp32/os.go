
package os

import (
	"syscall"
)

var (
	Stdin  = NewFile(uintptr(syscall.Stdin))
	Stdout = NewFile(uintptr(syscall.Stdout))
	Stderr = NewFile(uintptr(syscall.Stderr))
)

type File struct {
	fd uintptr
}

func NewFile(fd uintptr) *File {
	return &File{fd}
}

func (f *File) Write(b []byte) (int, error) {
	if f.fd == 1 || f.fd == 2 {
		print(string(b))
		return len(b), nil
	} else {
		return 0, syscall.EBADF
	}
}

func (f *File) Read(b []byte) (int, error) {
	return 0, syscall.EBADF // TODO
}
