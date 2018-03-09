
// Test the goroutine and channel implementation.
package main

// Numer of messages that takes about 10s on a Raspberry Pi 3
//const MESSAGES = 5000000
const MESSAGES = 10

func main() {
	println("create channel and goroutine")
	ch := make(chan int64)
	go sender(ch)

	var count, sum int64
	for n := range ch {
		sum += n
		count += 1
	}

	println("number of messages:", count)
	println("sum of messages:   ", sum)
}

func sender(ch chan int64) {
	for i := int64(0); i < MESSAGES; i++ {
		ch <- i
	}
	close(ch)
}
