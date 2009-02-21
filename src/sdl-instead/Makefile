include ../../Rules.make

CFLAGS += $(SDL_CFLAGS) $(LUA_CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\"  -DGAMES_PATH=\"${GAMESPATH}/\" -DTHEMES_PATH=\"${THEMESPATH}/\" -DVERSION=$(VERSION)

LDFLAGS += $(SDL_LFLAGS) $(LUA_LFLAGS)

SRC     := graphics.c input.c game.c main.c instead.c sound.c

OBJ     := $(patsubst %.c, %.o, $(SRC))

all: sdl-instead
PREFIX:=/usr/local

$(OBJ): %.o : %.c gui.h
	$(CC) -c $(<) $(I) $(CFLAGS)

sdl-instead:  $(OBJ)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 
#	strip sdl-instead

install:
	install -d -m 0755 $(BIN)
	install -m 0755 sdl-instead $(BIN)/sdl-instead

clean:
	rm -f *.o sdl-instead

