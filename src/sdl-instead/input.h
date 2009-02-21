#ifndef __INPUT_H__
#define __INPUT_H__

#define KEY_DOWN	1
#define KEY_UP		2
#define MOUSE_DOWN	3
#define MOUSE_UP	4
#define MOUSE_WHEEL_UP	5
#define MOUSE_WHEEL_DOWN 6
#define MOUSE_MOTION	7

struct inp_event {
	int 	type;
	int 	code;
	char 	*sym;
	int 	x;
	int 	y;
};

int input(struct inp_event *ev, int wait);

#endif
