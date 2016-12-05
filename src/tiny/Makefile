include ../../Rules.make.standalone
include ../../config.make

CFLAGS += $(LUA_CFLAGS) $(EXTRA_CFLAGS) -DSTEAD_PATH=\"${STEADPATH}/\" -DVERSION=\"$(VERSION)\"

LDFLAGS += $(LUA_LFLAGS) $(EXTRA_LDFLAGS)

INSTEAD_SRC	:= instead/instead.c instead/util.c instead/list.c instead/cache.c instead/idf.c instead/tinymt32.c instead/lfs.c
INSTEAD_H	:= instead/cache.h instead/system.h instead/instead.h instead/list.h instead/util.h instead/tinymt32.h instead/idf.h

SRC     := main.c $(INSTEAD_SRC)

H	:= $(INSTEAD_H)

OBJ     := $(patsubst %.c, %.o, $(SRC)) $(ADDOBJ)

all: instead stead tiny-instead$(EXE)

$(OBJ): %.o : %.c $(H)
	$(CC) -c $(<) $(I) $(CFLAGS) $(CPPFLAGS) -o $(@)

tiny-instead$(EXE):  $(OBJ) $(RESOURCES)
	$(CC) $(CFLAGS) $(^) $(LDFLAGS) -o $(@) 

instead:
	ln -s ../instead instead

stead:
	ln -s ../../stead stead

install:
	$(INSTALLD) $(DESTDIR)$(BIN)
	$(INSTALLB) tiny-instead$(EXE) $(DESTDIR)$(BIN)/tiny-instead$(EXE)

uninstall:
	$(RM) $(DESTDIR)$(BIN)/tiny-instead$(EXE)

clean:
	$(RM) -f instead/*.o *.o instead stead tiny-instead$(EXE)
