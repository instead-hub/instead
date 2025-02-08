include ../Rules.make
include ../config.make

CFLAGS += $(SDL_CFLAGS) $(LUA_CFLAGS) $(ZLIB_CFLAGS) $(EXTRA_CFLAGS)  -D_USE_SDL -DLANG_PATH=\"${LANGPATH}/\" -DSTEAD_PATH=\"${STEADPATH}/\"  -DGAMES_PATH=\"${GAMESPATH}/\" -DTHEMES_PATH=\"${THEMESPATH}/\" -DVERSION=\"$(VERSION)\" -DICON_PATH=\"${ICONPATH}/\" -fmax-errors=1

LDFLAGS += $(SDL_LFLAGS) $(LUA_LFLAGS) $(ZLIB_LFLAGS) $(EXTRA_LDFLAGS)

INSTEAD_SRC	:= instead/instead.c instead/util.c instead/list.c instead/cache.c instead/idf.c instead/tinymt32.c instead/lfs.c
INSTEAD_H	:= instead/cache.h instead/system.h instead/instead.h instead/list.h instead/util.h instead/tinymt32.h instead/idf.h

SRC     := graphics.c input.c game.c main.c \
	instead_bits.c instead_sprites.c instead_timer.c instead_sound.c instead_paths.c noise1234.c \
	sound.c SDL3_rotozoom.c SDL3_gfxBlitFunc.c SDL3_gif.c config.c themes.c menu.c \
	utils.c unzip.c ioapi.c unpack.c $(PLATFORM) $(INSTEAD_SRC)

H	:= config.h externals.h game.h graphics.h input.h internals.h ioapi.h iowin32.h \
	menu.h SDL3_rotozoom.h SDL3_gif.h SDL3_gfxBlitFunc.h sound.h themes.h unzip.h $(INSTEAD_H)

OBJ     := $(patsubst %.c, %.o, $(SRC)) $(ADDOBJ)

all: sdl-instead$(EXE)

$(OBJ): %.o : %.c $(H)
	$(CC) -c $(<) $(I) $(CFLAGS) $(CPPFLAGS) -o $(@)

sdl-instead$(EXE):  $(OBJ) $(RESOURCES)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 

install:
	$(INSTALLD) $(DESTDIR)$(BIN)
	$(INSTALLB) sdl-instead$(EXE) $(DESTDIR)$(BIN)/sdl-instead$(EXE)
	$(LN) sdl-instead$(EXE) $(DESTDIR)$(BIN)/instead$(EXE)

uninstall:
	$(RM) $(DESTDIR)$(BIN)/sdl-instead$(EXE)
	$(RM) $(DESTDIR)$(BIN)/instead$(EXE)

clean:
	$(RM) -f *.o instead/*.o sdl-instead$(EXE) 
