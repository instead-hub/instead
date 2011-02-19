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
void push_user_event(void (*p) (void*), void *data);
int mouse_focus(void);
extern int minimized(void);
extern int mouse_cursor(int on);

#endif
