LUA_CFLAGS=$(shell pkg-config --cflags lua)
LUA_LFLAGS=$(shell pkg-config --libs lua)
SDL_CFLAGS=$(shell sdl-config --cflags)
SDL_LFLAGS=$(shell sdl-config --libs) -lSDL_ttf -lSDL_mixer -lSDL_image
