package main

import (
	"bufio"
	"fmt"
	"../ifbot"
	"os"
)

func main() {
	scanner := bufio.NewScanner(os.Stdin)

	for scanner.Scan() {
		str := ifbot.Input(1, scanner.Text())
		fmt.Printf(str)
		ifbot.Cleanup()
	}
}
