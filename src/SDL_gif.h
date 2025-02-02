#ifndef _SDL_AGIF_H
#define _SDL_AGIF_H

/* Code adopted from SDL2_image library, Licensed under zlib */

/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* This is a GIF image file loading framework */

#include <SDL3/SDL.h>

typedef struct
{
	int w, h;
	int count;
	int loop;
	SDL_Surface **frames;
	int *delays;
} Animation_t;

extern SDL_DECLSPEC Animation_t *GIF_LoadAnim(const char* file);
extern SDL_DECLSPEC void FreeAnimation(Animation_t *anim);

#endif /* _SDL_AGIF_H */
