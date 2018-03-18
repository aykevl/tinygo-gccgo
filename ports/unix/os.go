
package os

import (
	"syscall"
)

var (
	Stdin  = &File{syscall.Stdin, "/dev/stdin"}
	Stdout = &File{syscall.Stdout, "/dev/stdout"}
	Stderr = &File{syscall.Stderr, "/dev/stderr"}
)

type File struct {
	fd   int
	name string
}

func (f *File) Write(b []byte) (int, error) {
	return syscall.Write(f.fd, b)
}

func (f *File) Read(b []byte) (int, error) {
	return syscall.Read(f.fd, b)
}
