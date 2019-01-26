/* golang metaparser3 telegram bot */

package main

import (
	"bufio"
	"github.com/Syfaro/telegram-bot-api"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"
)

type Instance struct {
	id        int64
	name      string
	input     chan string
	output    chan string
	last_time int64
}

var Servers []*Instance

func Input(id int64, str string) string {
	var msg string
	var new bool
	var ctx *Instance
	if ctx = Lookup(id); ctx == nil {
		new = true
		ctx, msg = Spawn(id)
	}

	if ctx == nil {
		return msg
	}

	if msg != "" {
		msg = msg + "\n"
	}

	cmd := strings.TrimSpace(strings.ToLower(str))

	if strings.HasPrefix(cmd, "load") || strings.HasPrefix(cmd, "save") {
		return ""
	} else if strings.HasPrefix(cmd, "/restart") ||
		strings.HasPrefix(cmd, "/stop") ||
		strings.HasPrefix(cmd, "/start") {
		if !new || strings.HasPrefix(cmd, "/restart") { // restart
			Stop(ctx)
			if strings.HasPrefix(cmd, "/restart") {
				os.Remove(savedir(ctx) + "/autosave")
			}
			if ctx, msg = Spawn(id); ctx == nil {
				return msg
			}
		}
		str = ""
	} else if strings.HasPrefix(cmd, "/help") {
		b, err := ioutil.ReadFile("./HELP")
		if err == nil {
			return string(b) + "\n"
		}
	}
	log.Printf("input: %s\n", str)
	ctx.input <- str

	ctx.last_time = time.Now().Unix()
	o := <-ctx.output

	if strings.HasPrefix(o, "%ERROR") {
		log.Printf("Error in game %d\n", id)
		Purge(ctx)
		return msg
	}
	return msg + o
}

func Cleanup() {
	var stop_Servers []*Instance
	t := time.Now().Unix()
	for _, v := range Servers {
		if t-v.last_time >= 60*15 {
			stop_Servers = append(stop_Servers, v)
		}
	}
	for _, v := range stop_Servers {
		log.Printf("Stopped %d after %d\n", v.id, t-v.last_time)
		Stop(v)
	}
}

func Purge(ctx *Instance) {
	for n, v := range Servers {
		if v.id == ctx.id {
			Servers[len(Servers)-1], Servers[n] = Servers[n], Servers[len(Servers)-1]
			Servers = Servers[:len(Servers)-1]
			return
		}
	}
}

func Kill() {
	t := time.Now().Unix()
	var oldest int64 = 0
	var ctx *Instance
	for _, v := range Servers {
		if t-v.last_time > oldest {
			oldest = t - v.last_time
			ctx = v
		}
	}
	if ctx != nil {
		log.Printf("Killing %d\n", ctx.id)
		Stop(ctx)
	}
}

func Stop(ctx *Instance) {
	ctx.input <- ("save " + autosave(ctx))
	log.Printf("%s\n", <-ctx.output)
	ctx.input <- "quit"
	log.Printf("%s\n", <-ctx.output)
	Purge(ctx)
}

func Lookup(id int64) *Instance {
	for k, v := range Servers {
		if v.id == id {
			return Servers[k]
		}
	}
	return nil
}

func Spawn(id int64) (*Instance, string) {
	msg := ""
	ctx := Lookup(id)
	if ctx != nil {
		return ctx, msg
	}

	if len(Servers) > 16 {
		Cleanup()
		if len(Servers) > 16 {
			Kill()
		}
	}

	ctx = new(Instance)
	ctx.input = make(chan string)
	ctx.output = make(chan string)
	ctx.id = id
	Servers = append(Servers, ctx)
	go server(ctx)
	msg = <-ctx.output
	//	fmt.Print(<-ctx.output)
	return ctx, msg
}

func savedir(ctx *Instance) string {
	return "./games/" + ctx.name + "/saves/" + strconv.FormatInt(ctx.id, 10)
}

func autosave(ctx *Instance) string {
	return "./saves/" + strconv.FormatInt(ctx.id, 10) + "/autosave"
}

func grab(b *bufio.Reader) (string, bool) {
	var res string
	for {
		result, _, err := b.ReadLine()
		if err != nil {
			return "", false
		}
		if string(result) == ">" {
			break
		}
		res = res + string(result) + "\n"
	}
	return res, true
}

func server(ctx *Instance) {
	input := ctx.input
	output := ctx.output

	var res string
	res = ""

	b, err := ioutil.ReadFile("./MOTD")
	if err == nil {
		res = string(b) + "\n"
	}

	files, err := ioutil.ReadDir("./games/")
	if err != nil {
		output <- "%ERROR 0"
		return
	}

	for k, f := range files {
		res = res + strconv.Itoa(k+1) + ") " + f.Name() + "\n"
	}
	for {
		output <- res
		num, err := strconv.Atoi(<-input)

		if err != nil {
			res = ""
			continue
		}
		num--
		if num < 0 || num >= len(files) {
			continue
		}
		ctx.name = files[num].Name()
		err = os.MkdirAll(savedir(ctx), os.ModePerm)
		if err != nil {
			continue
		}
		break
	}

	cmd := exec.Command("./tiny-mp", "./games/"+ctx.name)
	in, err := cmd.StdinPipe()
	if err != nil {
		output <- "%ERROR 1"
		return
	}
	defer in.Close()
	out, err := cmd.StdoutPipe()
	if err != nil {
		output <- "%ERROR 2"
		return
	}
	defer out.Close()

	cmd.Stderr = os.Stderr

	bufOut := bufio.NewReader(out)

	if err = cmd.Start(); err != nil {
		output <- "%ERROR 3"
		return
	}
	defer cmd.Wait()

	res, ok := grab(bufOut)

	if !ok {
		output <- "%ERROR 3"
		return
	}

	if _, err := os.Stat(savedir(ctx) + "/autosave"); err == nil {
		in.Write([]byte("load " + autosave(ctx) + "\n"))
		res, ok = grab(bufOut)
		if !ok {
			output <- "%ERROR 3"
			return
		}
	}
	output <- string(res)

	for {
		str := <-input
		_, err = in.Write([]byte(str + "\n"))
		if err != nil {
			output <- "%ERROR 4"
			return
		}

		res, ok = grab(bufOut)
		if !ok {
			output <- "%ERROR 5"
			return
		}
		output <- string(res)
	}
	output <- "%ERROR 0"
}

func main() {
	/*
		scanner := bufio.NewScanner(os.Stdin)

		for scanner.Scan() {
			str := Input(1, scanner.Text())
			fmt.Printf(str)
			Cleanup()
		}
	*/
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
		reply := Input(ChatID, Text)
		msg := tgbotapi.NewMessage(ChatID, reply)
		bot.Send(msg)
		Cleanup()
	}
}
