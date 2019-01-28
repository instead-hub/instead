/* golang metaparser3 telegram bot */
package main

import (
	"github.com/Syfaro/telegram-bot-api"
	"../ifbot"
	"log"
)

func main() {
	bot, err := tgbotapi.NewBotAPI("TOKEN")
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
