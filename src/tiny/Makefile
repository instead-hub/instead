include ../../Rules.make.standalone
include ../../config.make

CFLAGS += $(LUA_CFLAGS) $(EXTRA_CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\" -DVERSION=\"$(VERSION)\"

LDFLAGS += $(LUA_LFLAGS) $(EXTRA_LDFLAGS)

INSTEAD_SRC	:= instead.c util.c list.c cache.c idf.c tinymt32.c lfs.c

SRC     := $(INSTEAD_SRC)
OBJ     := $(patsubst %.c, %.o, $(SRC))

all: instead stead tiny-instead$(EXE) tiny-mp$(EXE)

$(OBJ): %.o : instead/%.c
	$(CC) -c $(<) $(I) $(CFLAGS) $(CPPFLAGS) -o $(@)

tiny-instead$(EXE): main.c $(OBJ)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@)

tiny-mp$(EXE): metaparser.c $(OBJ)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@)

instead:
	ln -s ../instead instead

stead:
	ln -s ../../stead stead

clean:
	$(RM) -f *.o instead stead tiny-instead$(EXE) tiny-mp$(EXE)
