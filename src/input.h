/*
 * Copyright 2009-2019 Peter Kosyh <p.kosyh at gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __INPUT_H__
#define __INPUT_H__

#define KEY_DOWN	1
#define KEY_UP		2
#define MOUSE_DOWN	3
#define MOUSE_UP	4
#define MOUSE_WHEEL_UP	5
#define MOUSE_WHEEL_DOWN 6
#define MOUSE_MOTION	7
#define USER_EVENT	8
#define FINGER_UP	9
#define FINGER_DOWN	10
#define KEY_TEXT	11

#define EV_CODE_KBD -1
#define EV_CODE_FINGER -2
#define EV_CODE_TEXT -3

#define AGAIN 2
struct inp_event {
	int 	type;
	int 	code;
	char 	sym[64];
	int 	x;
	int 	y;
	int 	count;
};

int input(struct inp_event *ev, int wait);
int input_init(void);
void input_clear(void);
void input_uevents(void);
void push_user_event(void (*p) (void*), void *data);
int mouse_focus(void);
extern int minimized(void);
extern int mouse_cursor(int on);
extern int finger_pos(const char *finger, int *x, int *y, float *pressure);
extern int system_clipboard(const char *text, char **buf);
extern int input_text(int start);

#endif
