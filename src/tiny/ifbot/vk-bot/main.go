/* golang metaparser3 vk bot */
package main

import (
	"../ifbot"
	"fmt"
	"github.com/Dimonchik0036/vk-api"
	"log"
	"os"
	"strings"
	"time"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("BOT Token missing")
		os.Exit(1)
	}
	client, err := vkapi.NewClientFromToken(os.Args[1])
	if err != nil {
		log.Panic(err)
	}

	// client.Log(true)
	if err := client.InitLongPoll(0, 2); err != nil {
		log.Panic(err)
	}
	updates, _, err := client.GetLPUpdatesChan(100, vkapi.LPConfig{25, vkapi.LPModeAttachments})
	if err != nil {
		log.Panic(err)
	}
	for update := range updates {
		if update.Message == nil || !update.IsNewMessage() || update.Message.Outbox() {
			continue
		}
		ID := update.Message.FromID
		Text := update.Message.Text
		//		log.Printf("Input: %s", Text)
		reply := ifbot.Input(ID, Text)
		//		log.Printf("Output: %s", reply)
		lines := strings.Split(reply, "\n")
		chunk := ""
		for _, l := range lines {
			if len(chunk)+len(l) > 1024 {
				var first bool
				if chunk == "" {
					chunk = l + "\n"
					first = true
				}
				for count := 3; count > 0; count-- {
					rc, _ := client.SendMessage(vkapi.NewMessage(vkapi.NewDstFromUserID(ID), chunk))
					if rc > 0 {
						break
					}
					time.Sleep(time.Second / 2.0)
				}
				if first {
					chunk = ""
				} else {
					chunk = l + "\n"
				}
			} else {
				chunk += (l + "\n")
			}
		}
		if chunk != "" {
			client.SendMessage(vkapi.NewMessage(vkapi.NewDstFromUserID(ID), chunk))
		}
		ifbot.Cleanup()
	}
}
