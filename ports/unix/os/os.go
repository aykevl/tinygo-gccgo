
package os

import (
	"syscall"
)

var (
	Stdin  = NewFile(uintptr(syscall.Stdin), "/dev/stdin")
	Stdout = NewFile(uintptr(syscall.Stdout), "/dev/stdout")
	Stderr = NewFile(uintptr(syscall.Stderr), "/dev/stderr")
)

type File struct {
	fd   uintptr
	name string
}

func NewFile(fd uintptr, name string) *File {
	return &File{fd, name}
}

func (f *File) Write(b []byte) (int, error) {
	return syscall.Write(1, b) // TODO fileno
}
