include ../../Rules.make

CFLAGS += $(SDL_CFLAGS) $(LUA_CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\"  -DGAMES_PATH=\"${GAMESPATH}/\" -DTHEMES_PATH=\"${THEMESPATH}/\" -DVERSION=$(VERSION) -DICON_PATH=\"${ICONPATH}/\"

LDFLAGS += $(SDL_LFLAGS) $(LUA_LFLAGS)

SRC     := graphics.c input.c game.c main.c instead.c sound.c SDL_rotozoom.c $(PLATFORM)

OBJ     := $(patsubst %.c, %.o, $(SRC))

all: sdl-instead$(EXE)

$(OBJ): %.o : %.c gui.h menu.h menu-en.h
	$(CC) -c $(<) $(I) $(CFLAGS)

sdl-instead$(EXE):  $(OBJ) $(RESOURCES)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 

install:
	$(INSTALLD) $(BIN)
	$(INSTALL) sdl-instead$(EXE) $(BIN)/sdl-instead$(EXE)

clean:
	$(RM) -f *.o sdl-instead$(EXE)
