#!/usr/bin/make -f
include ../../Rules.make
include ../../config.make

all: instead
instead: instead.c rline.c
	$(CC) $(CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\" instead.c $(LUA_CFLAGS) rline.c $(LUA_LFLAGS) -lncurses -lreadline -o instead
clean:
	$(RM) -rf *.o instead
install:
	$(INSTALLD) $(BIN)
	$(INSTALLB) instead $(BIN)/instead

uninstall:
	$(RM) $(BIN)/instead

