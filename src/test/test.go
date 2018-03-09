
package main

import (
	//"sample_go_package"
	"unicode"
	"fmt"
	"strconv"
)

func foo(ch chan int) {
	println("hello world!")
	defer func() {
		println("check me running anyway!")
	}()
	//ch <- 42
	for i := 0; i < 3; i++ {
		ch <- 1
		if i == 2 {
			//panic("hello")
		}
	}
	close(ch)
}

func late(i int) {
	println("Late function called:", i)
}

func Bar() {
	defer late(1)
	defer late(2)
	i := "asdf"
	defer func(j int) {
		println("inside closure!", i)
		//if r := recover(); r != nil {
		//	println("recovered:", r)
		//} else {
		//	println("no panic to recover")
		//}
	}(5);
	//panic("panic to recover")
	i = "jkl"
}

func testMap() {
	m := make(map[string]int)
	m["number"] = 42
	for k, v := range m {
		println("map k, v:", k, v)
	}
	println("value:", m["number"])
	delete(m, "number")
	println("after deletion, map has", len(m), "members")
}

func main() {
	println("start")
	Bar()
	testMap()
	println("unicode.ToUpper:", 'a', "->", unicode.ToUpper('a'))
	defer late(3)
	_ = fmt.Sprintf
	//var slice []int
	slice := make([]int, 0, 2)
	ch := make(chan int)
	go foo(ch)
	answer := 0
	for i := range ch {
		answer += i
		println("i:", i, answer)
		slice = append(slice, i)
	}
	println("answer:", strconv.Itoa(answer))
	println("answer:", answer, len(slice))
	//println("answer2:", fmt.Sprintf("%d", answer))
	//for _, i := range slice {
	//	println("i2:", i)
	//}
}
