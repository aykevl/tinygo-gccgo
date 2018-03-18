
package syscall

const (
	EPERM  = Errno(1)
	ENOENT = Errno(2)
	EIO    = Errno(5)
	EBADF  = Errno(9)
)

var (
	Stdin  = 0
	Stdout = 1
	Stderr = 2
)

func Getenv(key string) (value string, found bool) {
	return "", false
}

type Errno uintptr

func (e Errno) Error() string {
	return "errno: " + string(e)
}
