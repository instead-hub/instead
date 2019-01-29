/* golang metaparser3 telegram bot */
package main

import (
	"../ifbot"
	"fmt"
	"github.com/Syfaro/telegram-bot-api"
	"log"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("BOT Token missing")
		os.Exit(1)
	}
	bot, err := tgbotapi.NewBotAPI(os.Args[1])
	if err != nil {
		log.Panic(err)
	}

	// bot.Debug = true
	log.Printf("Authorized on account %s", bot.Self.UserName)

	u := tgbotapi.NewUpdate(0)
	u.Timeout = 60

	updates, err := bot.GetUpdatesChan(u)
	for update := range updates {
		if update.Message == nil {
			continue
		}
		ChatID := update.Message.Chat.ID
		Text := update.Message.Text
		reply := ifbot.Input(ChatID, Text)
		msg := tgbotapi.NewMessage(ChatID, reply)
		bot.Send(msg)
		ifbot.Cleanup()
	}
}
