
package syscall

func Getenv(key string) (value string, found bool) {
	return "", false
}

type Errno uintptr

func (e Errno) Error() string {
	return "errno: " + string(e)
}

const ENOENT = Errno(2)
