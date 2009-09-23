#!/usr/bin/make -f
include ../../Rules.make
all: instead
instead: instead.c rline.c
	$(CC) $(CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\" instead.c $(LUA_CFLAGS) rline.c $(LUA_LFLAGS) -lncurses -lreadline -o instead
clean:
	$(RM) -rf *.o instead
install:
	$(INSTALLD) $(BIN)
	$(INSTALLB) instead $(BIN)/instead

ifeq ($(SYSTEMSETUP), yes)
uninstall:
	$(RM) $(BIN)/instead
else
uninstall:
endif
