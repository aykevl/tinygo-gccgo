
// Test various Go types
package main

func main() {
	println("test slice:")
	testSlice()
	println("\ntest map:")
	testMap()
}

func testSlice() {
	slice := make([]int, 0, 2)
	for i := 0; i < 10; i++ {
		slice = append(slice, i)
	}

	print("slice contains:")
	for _, n := range slice {
		print(" ", n)
	}
	println()
	println("slice has", len(slice), "members and a capacity of", cap(slice))
}

func testMap() {
	m := make(map[string]int)
	m["number"] = 42
	for k, v := range m {
		println("map has member", k, "with value", v)
	}

	println("value:", m["number"])

	// check map access
	if _, ok := m["number"]; !ok {
		panic("map contains no \"number\"?")
	}
	if _, ok := m["foo"]; ok {
		panic("map contains \"foo\"?")
	}

	delete(m, "number")
	println("after deletion, map has", len(m), "members")
}
