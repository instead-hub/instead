#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>

#include <termios.h>
#include <stdio.h>
#include <wchar.h>
#ifdef _HAVE_ICONV
#include <langinfo.h>
#include <iconv.h>
#endif

extern char *getstring(char *cmd);

char *stripwhite(char *string)
{
	char *s, *t;

	for (s = string; isspace(*s); s++);

	if (*s == 0)
		return (s);

	t = s + strlen(s) - 1;
	while (t > s && isspace(*t))
		t--;
	*++t = '\0';

	return s;
}

#define MAX_WORDS 255
struct completion;
struct completion {
	char *list;
	char *words[MAX_WORDS + 1];
	int   ids[MAX_WORDS + 1];
	int 	num;
	int 	period;
	int 	id;
	int	hide;
	struct completion *next;
};

struct completion obj_comp = { .list = NULL, .id = 1,};
struct completion way_comp = { .list = NULL, .id = 1,};
struct completion inv_comp = { .list = NULL, .period = 1, .id = 1,};
struct completion cmd_comp = { .list = NULL, .hide = 1,};

int comp_num(struct completion *c) {
	int num = 0;
	for (; c && !c->hide; c = c->next) {
		num += c->num;
	}
	return num;
}

char *comp_generator(const char *text, int state, struct completion *co)
{
	struct completion *c = co;
	static int list_index;
	int len;
	char *name;
	int i = 0, id;
	int index = 0;
	if (!state) {
		list_index = 0;
	}
	len = strlen(text);
	for (; c && (index != list_index);) {
		for (i = 0; (index != list_index) && c->words[i]; i ++) 
			index ++;
		if (index != list_index)
			c = c->next;
	}
	for (;c; c = c->next) {
		if (c->hide && !len)
			continue;
		id =  (c->id)?atoi(text):0;
		if (id) {
			if (state)
				continue;
		}
		for (; (name = c->words[i]); i ++) {
			name = c->words[i];	
			list_index ++;
			if (id == c->ids[i] || strncmp(name, text, len) == 0) {
				int expand = 0;
				char *s = malloc(strlen(name) + 16);
				if (!s)
					return NULL;
				*s = 0;
				if ( len == 0 && !id) {
					if (comp_num(co) >1 && c->id) {
						expand = 1;
						sprintf(s, "%02d:", c->ids[i]);
					}
				}
				strcat(s, name);
				if (c->period) {
					if (expand)
						strcat(s, "(*)");
					else if (c->period == 1)
						strcat(s, ",");
				} else
					strcat(s, " ");
				return s;
			}
		}
		i = 0;
	}
	return NULL;
}

void getcompletion(char *list, struct completion *c)
{
	int i;
	char *p;
	char *s = list;
	c->num = 0;
	if (c->list)
		free(c->list);
	c->list = list;
	for (i = 0; list && i < MAX_WORDS && *s; i++) {
		while (isspace(*s))
			s ++;
		if (!*s)
			break;
		c->words[i] = s;
		c->num ++;
		s += strcspn(s, ",(");
		c->ids[i] = -1;
		if (*s == '(') {
			p = s;
			s ++;
			c->ids[i] = atoi(s);
			s += strcspn(s, ",)");
			if (*s == ')')
				s ++;
			*p = 0;
		}
		if (*s == ',') {
			*s = 0;
			s ++;
		}
	}
	c->words[i] = NULL;
	return;
}

char *obj_generator(const char *text, int state)
{
	obj_comp.next = NULL;
	return comp_generator(text, state, &obj_comp);
}

char *way_generator(const char *text, int state)
{
	return comp_generator(text, state, &way_comp);
}

char *inv_generator(const char *text, int state)
{
	inv_comp.next = NULL;	
	inv_comp.period = 1;
	return comp_generator(text, state, &inv_comp);
}

char *objcmd_generator(const char *text, int state)
{
	obj_comp.next = &cmd_comp;
	return comp_generator(text, state, &obj_comp);
}

char *objinv_generator(const char *text, int state)
{
	obj_comp.next = &inv_comp;
	inv_comp.period = -1;
	return comp_generator(text, state, &obj_comp);
}


char *search_word(char *text, char **words, int n, int len)
{
	int i;
	char *t = strdup(text);
	if (!t)
		return NULL;
	t[len] = 0;
	while (isspace(*text))
		text ++;
	for (i = 0; i < n; i++) {
		if (!strncmp(text, words[i], strlen(words[i]))) {
			free(t);
			return words[i];
		}
	}
	free(t);
	return NULL;
}

char **tquest_completion(char *text, int start, int end)
{
	char **matches;
	rl_attempted_completion_over = 1;
	matches = (char **) NULL;
	/* If this word is at the start of the line, then it is a command
	   to complete.  Otherwise it is the name of a file in the current
	   directory. */
	//str = strdup(rl_line_buffer);
	if (start == 0)
		matches = rl_completion_matches(text, objcmd_generator);
	else {
		char *n = search_word(rl_line_buffer, cmd_comp.words, cmd_comp.num, rl_point);
		if (!n) 
			return (matches);
		else if (!strcmp(n, "go"))
			matches = rl_completion_matches(text, way_generator);
		else if (!strcmp(n, "use")) {
			if (!strstr(rl_line_buffer,",")) {
				matches = rl_completion_matches(text, inv_generator);
			} else {
				matches = rl_completion_matches(text, objinv_generator);
			}
		} else if (!strcmp(n, "act")) {
			matches = rl_completion_matches(text, obj_generator);
		} else if (!strcmp(n, "load") || !strcmp(n, "save")) {
			rl_attempted_completion_over = 0;
		} 
	}
	return (matches);
}

/* Tell the GNU Readline library how to complete.  We want to try to complete
    on command names if this is the first word in the line, or on filenames
    if not. */
#ifdef _HAVE_ICONV
static char curcp[64];
static char *fromcp;
#endif
void initialize_readline(void)
{
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "instead";
	rl_completion_append_character=0;
	rl_basic_word_break_characters = " \t,";
	rl_parse_and_bind(strdup("set show-all-if-ambiguous on"));
	rl_parse_and_bind(strdup("Control-u: 'use '"));
	rl_parse_and_bind(strdup("Control-a: 'act '"));
	rl_parse_and_bind(strdup("Control-g: 'go '"));
	/* Tell the completer that we want a crack first. */
	rl_attempted_completion_function = (CPPFunction *) tquest_completion;
#ifdef _HAVE_ICONV
	strncpy(curcp, nl_langinfo(CODESET), sizeof(curcp));
	fromcp = getstring("return game.codepage;");
#endif
}

wchar_t *string_towc(const char *str)
{
	wchar_t *pwstr=NULL;
	size_t size;
	if (!str)
		return NULL;
	pwstr = malloc((1 + strlen(str)) * sizeof(wchar_t));
	size = mbstowcs(pwstr, str, strlen(str));
	if (size == (size_t)-1) {
		free(pwstr);
		return NULL;
	}	
	pwstr[size] = 0;
	return pwstr;
}

char *string_fromwc(const wchar_t *str)
{
	char *pstr=NULL;
	size_t mbs_size;
	if (!str)
		return NULL;
	mbs_size = wcstombs(NULL, str, 0);
	if (mbs_size == (size_t)-1)
		return NULL;
	pstr = malloc(mbs_size + 1);
	pstr[ wcstombs(pstr, str, mbs_size) ] = 0;
	return pstr;
}
#ifdef _HAVE_ICONV
#define CHAR_MAX_LEN 4
static char *decode(iconv_t hiconv, const char *s)
{
	size_t s_size, chs_size, outsz, insz;
	char *inbuf, *outbuf, *chs_buf;
	if (!s || hiconv == (iconv_t)(-1))
		return NULL;
	s_size = strlen(s); 
	chs_size = s_size * CHAR_MAX_LEN; 
	if ((chs_buf = malloc(chs_size + CHAR_MAX_LEN))==NULL)
		goto exitf; 
	outsz = chs_size; 
	outbuf = chs_buf; 
	insz = s_size; 
	inbuf = (char*)s; 
	while (insz) { 
		if (iconv(hiconv, &inbuf, &insz, &outbuf, &outsz) 
						== (size_t)(-1)) 
	   	 	goto exitf; 
	} 
	*outbuf++ = 0; 
	return chs_buf; 
exitf: 
	if(chs_buf) 
		free(chs_buf); 
	return NULL; 
}

char *fromgame(const char *s)
{
	iconv_t han;
	char *str;
	if (!s)
		return NULL;
	if (!fromcp)
		goto out0;
	han = iconv_open(curcp, fromcp);
	if (han == (iconv_t)-1)
		goto out0;
	if (!(str = decode(han, s)))
		goto out1;
	iconv_close(han);
	return str;
out1:
	iconv_close(han);
out0:
	return strdup(s);
}

char *togame(const char *s)
{
	iconv_t han;
	char *str;
	if (!s)
		return NULL;
	if (!fromcp)
		goto out0;
	han = iconv_open(fromcp, curcp);
	if (han == (iconv_t)-1)
		goto out0;
	if (!(str = decode(han, s)))
		goto out1;
	iconv_close(han);
	return str;
out1:
	iconv_close(han);
out0:
	return strdup(s);
}
#else
char *fromgame(const char *s) 
{
	if (!s)
		return NULL;
	return strdup(s);
}
char *togame(const char *s) 
{
	if (!s)
		return NULL;
	return strdup(s);
}
#endif

int width = 80;
int height = 25;
#define STDIN_FILENO 0

int getch(void) {
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

void out(char *str)
{
	int lines = 0;
	int i;
	
	wchar_t *ws, *wp, saved, *ows;
	char *ms;
	size_t	sz = width, oldsz;
	if (!str)
		return;
	ows = ws = string_towc(str);
	if (!ws)
		return;
	wp = ws;
	fprintf(stdout,"\n");
	for (i = 0; i < width/2 - 6; i++)
	fprintf(stdout, " ");
	fprintf(stdout,"* * *\n\n");

	while (*ws && *wp) {
		wp += wcscspn(wp, L" ,\t\n");
		if (*wp == ',')
			wp ++;
		saved = *wp;
		*wp = 0;
		oldsz = sz;
		sz = wcslen(ws);
		if (sz > width || saved == '\n') {
			*wp = saved;
			if (sz <= width)
				oldsz = sz;
			saved = ws[oldsz];
			ws[oldsz] = 0;
			lines ++;
			if (lines >= (height)) {
				printf("-- more --");
				getch();
				printf("\r                \r");
				lines = 0;
			}
			ms = string_fromwc(ws);
			if (ms)
				printf("%s\n", ms);
			free(ms);
			ws[oldsz] = saved;
			if (saved == L' ' || saved == L'\t' || saved == L'\n')
				oldsz ++;
			ws += oldsz;
			wp = ws;
			sz = width;
			continue;
		}
		*wp = saved;
		if (saved)
			wp ++;
	}
	if (wp != ws) {
		lines ++;
		if (lines > height) {
			printf("-- more --");
			getch();
			printf("\r");
			lines = 0;
		}
		ms = string_fromwc(ws);
		if (ms)
			printf("%s\n", ms);
		free(ms);
	}
	free(ows);
	free(str);
}

void execute_line(char *s)
{
	char buf[1024];
	char *p = s;
	while (*p) {
		if (*p == '\\' || *p == '\'' || *p == '\"' || *p == '[' || *p == ']')
			return;
		p ++;
	}
	s = togame(s);
	snprintf(buf, sizeof(buf), "return iface:cmd('%s')", s);
	p = getstring(buf);
	free(s);
	out(p);
}
extern int luacall(char *cmd);

void loop(void)
{
	char *line, *s;

	initialize_readline();	/* Bind our completer. */
	getcompletion(strdup("act,look,go,back,use,load,save,quit,help,obj,inv,way,ls"), &cmd_comp);
	out(getstring("return (par('',fmt(game:ini()),iface:cmd('look')));"));
//	out(getstring("return fmt(me():look());"));
	/* Loop reading and executing lines until the user quits. */
	while (1) {
		luacall("me():tag();");
		getcompletion(getstring("return here():str();"), &obj_comp);
		getcompletion(getstring("return me():str();"), &inv_comp);
		getcompletion(getstring("return here().way:str();"), &way_comp);
		line = readline("-- > ");

		if (!line)
			break;

		/* Remove leading and trailing whitespace from the line.
		   Then, if there is anything left, add it to the history list
		   and execute it. */
		s = stripwhite(line);
		
		if (*s) {
			add_history(s);
		}
		if (!strcmp(s, "help")) {
			printf("Usage: act, look, use, go, back, load, save, quit.\n");
		} else if (!strcmp(s, "quit"))
			break;
		else {
			execute_line(s);
		}
		free(line);
	}
	exit(0);
}


