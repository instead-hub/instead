#ifndef _SDL_ICONV_H
#define _SDL_ICONV_H

#ifdef _HAVE_ICONV
#include <SDL_config.h>

#if defined(HAVE_ICONV) && !defined(HAVE_ICONV_H)
#include <SDL_stdinc.h>
#define iconv SDL_iconv
#define iconv_t SDL_iconv_t
#define iconv_open SDL_iconv_open
#define iconv_close SDL_iconv_close

#else

#include <iconv.h>

#endif /* _USE_SDL_ICONV */

#endif /* _HAVE_ICONV */

#endif /* _SDL_ICONV_H */

