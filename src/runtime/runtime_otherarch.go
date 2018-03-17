
package runtime

const GOARCH = "other"

type stackframe struct {
	sp int
	pc uintptr
}

func unwind_frame(frame *stackframe) string {
	return "unimplemented"
}
