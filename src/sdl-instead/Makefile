include ../../Rules.make
include ../../config.make

CFLAGS += $(SDL_CFLAGS) $(LUA_CFLAGS) $(ZLIB_CFLAGS) $(EXTRA_CFLAGS) -DLANG_PATH=\"${LANGPATH}/\" -DSTEAD_PATH=\"${STEADPATH}/\"  -DGAMES_PATH=\"${GAMESPATH}/\" -DTHEMES_PATH=\"${THEMESPATH}/\" -DVERSION=\"$(VERSION)\" -DICON_PATH=\"${ICONPATH}/\"

LDFLAGS += $(SDL_LFLAGS) $(LUA_LFLAGS) $(ZLIB_LFLAGS) $(EXTRA_LDFLAGS)
 
SRC     := graphics.c input.c game.c main.c instead.c sound.c SDL_rotozoom.c SDL_anigif.c SDL_gfxBlitFunc.c config.c themes.c menu.c util.c cache.c unzip.c ioapi.c unpack.c lfs.c idf.c $(PLATFORM)
H	:= cache.h config.h externals.h game.h graphics.h input.h instead.h internals.h ioapi.h iowin32.h list.h \
	menu.h SDL_anigif.h SDL_gfxBlitFunc.h SDL_rotozoom.h sound.h themes.h unzip.h util.h

OBJ     := $(patsubst %.c, %.o, $(SRC)) $(ADDOBJ)

all: sdl-instead$(EXE)

$(OBJ): %.o : %.c $(H)
	$(CC) -c $(<) $(I) $(CFLAGS)

sdl-instead$(EXE):  $(OBJ) $(RESOURCES)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 

install:
	$(INSTALLD) $(BIN)
	$(INSTALLB) sdl-instead$(EXE) $(BIN)/sdl-instead$(EXE)
	$(LN) sdl-instead$(EXE) $(BIN)/instead$(EXE)

uninstall:
	$(RM) $(BIN)/sdl-instead$(EXE)
	$(RM) $(BIN)/instead$(EXE)

clean:
	$(RM) -f *.o sdl-instead$(EXE)
