
package runtime

func Panic(msg interface{}, fatal bool) {
	print("panic: ")
	Printany(msg)
	println()

	// Panic is unimplemented.
	// Exit the process, or reset the chip.
	tinygo_fatal()
}
