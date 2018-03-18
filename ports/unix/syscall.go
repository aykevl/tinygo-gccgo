
package syscall

import (
	"unsafe"
)

func write(fd int, buf unsafe.Pointer, count uint) int __asm__("write")

func Write(fd int, p []byte) (int, error) {
	n := write(fd, unsafe.Pointer(&p[0]), uint(len(p)))
	if n == -1 {
		return 0, EPERM // TODO: find out which error it is
	}
	return n, nil
}
