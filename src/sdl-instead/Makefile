include ../../Rules.make
include ../../config.make

CFLAGS += $(SDL_CFLAGS) $(LUA_CFLAGS) -DLANG_PATH=\"${LANGPATH}/\" -DSTEAD_PATH=\"${STEADPATH}/\"  -DGAMES_PATH=\"${GAMESPATH}/\" -DTHEMES_PATH=\"${THEMESPATH}/\" -DVERSION=\"$(VERSION)\" -DICON_PATH=\"${ICONPATH}/\"

LDFLAGS += $(SDL_LFLAGS) $(LUA_LFLAGS)

SRC     := graphics.c input.c game.c main.c instead.c sound.c SDL_rotozoom.c SDL_anigif.c SDL_gfxBlitFunc.c config.c themes.c menu.c util.c $(PLATFORM)

OBJ     := $(patsubst %.c, %.o, $(SRC))

all: sdl-instead$(EXE)

$(OBJ): %.o : %.c menu.h config.h game.h themes.h externals.h internals.h util.h
	$(CC) -c $(<) $(I) $(CFLAGS)

sdl-instead$(EXE):  $(OBJ) $(RESOURCES)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 

install:
	$(INSTALLD) $(BIN)
	$(INSTALLB) sdl-instead$(EXE) $(BIN)/sdl-instead$(EXE)

uninstall:
	$(RM) $(BIN)/sdl-instead$(EXE)

clean:
	$(RM) -f *.o sdl-instead$(EXE)
