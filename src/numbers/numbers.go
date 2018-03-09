
// Print various numbers using the strconv library.
// This tests whether imports function properly.
package main

import (
	"strconv"
)

const number = 42

func main() {
	println("number printed regulary:                  ", number)
	println("number printed using strconv, decimal:    ", strconv.Itoa(number))
	println("number printed using strconv, hexadecimal:", strconv.FormatInt(number, 16))
	println("number printed using strconv, octal:      ", strconv.FormatInt(number, 8))
}
