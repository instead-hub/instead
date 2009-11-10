/* 

 SDL_gfxBlitFunc: custom blitters (part of SDL_gfx library)

 LGPL (c) A. Schiffler
 
*/

#ifndef _SDL_gfxBlitFunc_h
#define _SDL_gfxBlitFunc_h

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern    "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_video.h>

int  SDL_gfxBlitRGBA(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);

int SDL_gfxSetAlpha(SDL_Surface * src, Uint8 a);


/* -------- Macros */

/* Define SDL macros locally as a substitute for a #include "SDL_blit.h", */

/* which doesn't work since the include file doesn't get installed.       */

/* The structure passed to the low level blit functions */
  typedef struct {
    Uint8    *s_pixels;
    int       s_width;
    int       s_height;
    int       s_skip;
    Uint8    *d_pixels;
    int       d_width;
    int       d_height;
    int       d_skip;
    void     *aux_data;
    SDL_PixelFormat *src;
    Uint8    *table;
    SDL_PixelFormat *dst;
  } SDL_gfxBlitInfo;

#define GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a)				\
{									\
	r = ((pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss; 		\
	g = ((pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss; 		\
	b = ((pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss; 		\
	a = ((pixel&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss;	 	\
}

#define GFX_DISEMBLE_RGBA(buf, bpp, fmt, pixel, r, g, b, a)			   \
do {									   \
	pixel = *((Uint32 *)(buf));			   		   \
	GFX_RGBA_FROM_PIXEL(pixel, fmt, r, g, b, a);			   \
	pixel &= ~fmt->Amask;						   \
} while(0)

#define GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a)				\
{									\
	pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
		((g>>fmt->Gloss)<<fmt->Gshift)|				\
		((b>>fmt->Bloss)<<fmt->Bshift)|				\
		((a<<fmt->Aloss)<<fmt->Ashift);				\
}

#define GFX_ASSEMBLE_RGBA(buf, bpp, fmt, r, g, b, a)			\
{									\
			Uint32 pixel;					\
									\
			GFX_PIXEL_FROM_RGBA(pixel, fmt, r, g, b, a);	\
			*((Uint32 *)(buf)) = pixel;			\
}

/* Blend the RGB values of two pixels based on a source alpha value */
#define GFX_ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
do {						\
	dR = (((sR-dR)*(A))/255)+dR;		\
	dG = (((sG-dG)*(A))/255)+dG;		\
	dB = (((sB-dB)*(A))/255)+dB;		\
} while(0)

/* This is a very useful loop for optimizing blitters */

/* 4-times unrolled loop */
#define GFX_DUFFS_LOOP4(pixel_copy_increment, width)			\
{ int n = (width+3)/4;							\
	switch (width & 3) {						\
	case 0: do {	pixel_copy_increment;				\
	case 3:		pixel_copy_increment;				\
	case 2:		pixel_copy_increment;				\
	case 1:		pixel_copy_increment;				\
		} while ( --n > 0 );					\
	}								\
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _SDL_gfxBlitFunc_h */
