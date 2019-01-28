/* golang metaparser3 bot package */
package ifbot

import (
	"bufio"
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

var servers []*Instance

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
	//	log.Printf("input: %s\n", str)
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
	var stop_servers []*Instance
	t := time.Now().Unix()
	for _, v := range servers {
		if t-v.last_time >= 60*15 {
			stop_servers = append(stop_servers, v)
		}
	}
	for _, v := range stop_servers {
		log.Printf("Stopped %d after %d\n", v.id, t-v.last_time)
		Stop(v)
	}
}

func Purge(ctx *Instance) {
	for n, v := range servers {
		if v.id == ctx.id {
			servers[len(servers)-1], servers[n] = servers[n], servers[len(servers)-1]
			servers = servers[:len(servers)-1]
			return
		}
	}
}

func Kill() {
	t := time.Now().Unix()
	var oldest int64 = 0
	var ctx *Instance
	for _, v := range servers {
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
	for k, v := range servers {
		if v.id == id {
			return servers[k]
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

	if len(servers) > 16 {
		Cleanup()
		if len(servers) > 16 {
			Kill()
		}
	}

	ctx = new(Instance)
	ctx.input = make(chan string)
	ctx.output = make(chan string)
	ctx.id = id
	servers = append(servers, ctx)
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

	for {
		output <- res
		num, err := strconv.Atoi(<-input)

		if err != nil {
			res = ""
			for k, f := range files {
				res = res + strconv.Itoa(k+1) + ") " + f.Name() + "\n"
			}
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
		i := strings.Index(str, "\n")
		if i > -1 {
			str = str[:i]
		}
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
