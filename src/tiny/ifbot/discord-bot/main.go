package main

import (
	"../ifbot"
	"flag"
	"fmt"
	"github.com/bwmarrin/discordgo"
	"hash/fnv"
	"os"
	"os/signal"
	"strings"
	"syscall"
)

// Variables used for command line parameters
var (
	Token        string
	Channels     string
	ChannelsList []string
)

func init() {

	flag.StringVar(&Token, "t", "", "Bot Token")
	flag.StringVar(&Channels, "c", "", "Channels")
	flag.Parse()
}

func main() {
	ChannelsList = strings.Split(Channels, ",")

	ifbot.SaveDir = "discord-saves/"
	// Create a new Discord session using the provided bot token.
	dg, err := discordgo.New("Bot " + Token)
	if err != nil {
		fmt.Println("error creating Discord session,", err)
		return
	}

	// Register the messageCreate func as a callback for MessageCreate events.
	dg.AddHandler(messageCreate)

	// Open a websocket connection to Discord and begin listening.
	err = dg.Open()
	if err != nil {
		fmt.Println("error opening connection,", err)
		return
	}

	// Wait here until CTRL-C or other term signal is received.
	fmt.Println("Bot is now running.  Press CTRL-C to exit.")
	sc := make(chan os.Signal, 1)
	signal.Notify(sc, syscall.SIGINT, syscall.SIGTERM, os.Interrupt, os.Kill)
	<-sc

	// Cleanly close down the Discord session.
	dg.Close()
}

func hash(s string) int64 {
	h := fnv.New32a()
	h.Write([]byte(s))
	return int64(h.Sum32())
}

func isPrivate(s *discordgo.Session, m *discordgo.MessageCreate) bool {
	channel, err := s.State.Channel(m.ChannelID)
	if err != nil {
		if channel, err = s.Channel(m.ChannelID); err != nil {
			return false
		}
	}
	return channel.Type == discordgo.ChannelTypeDM
}

func isChannel(s *discordgo.Session, m *discordgo.MessageCreate) bool {
	if len(ChannelsList) == 0 {
		return true
	}
	for _, id := range ChannelsList {
		if m.ChannelID == id {
			return true
		}
	}
	return false
}

func messageCreate(s *discordgo.Session, m *discordgo.MessageCreate) {

	// Ignore all messages created by the bot itself
	// This isn't required in this specific example but it's a good practice.
	if m.Author.ID == s.State.User.ID {
		return
	}
	text := m.Content

	if !(isChannel(s, m) || isPrivate(s, m)) {
		return
	}
	reply := ifbot.Input(hash(m.ChannelID), text)
	s.ChannelMessageSend(m.ChannelID, reply)
	ifbot.Cleanup()
}
