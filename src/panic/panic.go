
// Test panic functions, and whether defer works properly.
package main

func main() {
	println("main: enter")
	defer func() {
		println("main: deferred function")
	}()
	Foo()
	println("main: exit")
}

func Foo() {
	println("Foo: enter")
	defer func() {
		println("Foo: deferred function")
	}()
	Bar()
	println("Foo: exit")
}

func Bar() {
	println("Bar: enter")
	defer func() {
		println("Bar: deferred function")
	}()
	panic("boom!")
	println("Bar: exit")
}
