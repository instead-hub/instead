#!/usr/bin/make -f
include ../../Rules.make
all: instead
instead: instead.c rline.c
	$(CC) $(CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\" instead.c $(LUA_CFLAGS) rline.c $(LUA_LFLAGS) -lreadline -o instead
clean:
	rm -rf *.o instead
install:
	install -d -m 0755 $(BIN)
	install -m 0755 instead $(BIN)/instead

