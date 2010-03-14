/*  

  SDL_rotozoom.c - rotozoomer for 32bit or 8bit surfaces

  LGPL (c) A. Schiffler

*/

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "SDL_rotozoom.h"

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))


/* 
 
 32bit integer-factor averaging Shrinker

 Shrinks 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

int shrinkSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int factorx, int factory)
{
    int x, y, dx, dy, sgap, dgap, ra, ga, ba, aa;
    int n_average;
    tColorRGBA *sp, *osp, *oosp;
    tColorRGBA *dp;

    /*
     * Averaging integer shrink
     */

    /* Precalculate division factor */
    n_average = factorx*factory;
   
    /*
     * Scan destination
     */
    sp = (tColorRGBA *) src->pixels;
    sgap = src->pitch - src->w * 4;

    dp = (tColorRGBA *) dst->pixels;
    dgap = dst->pitch - dst->w * 4;

    for (y = 0; y < dst->h; y++) {

      osp=sp;
      for (x = 0; x < dst->w; x++) {

        /* Trace out source box and accumulate */
        oosp=sp;
        ra=ga=ba=aa=0;
        for (dy=0; dy < factory; dy++) {
         for (dx=0; dx < factorx; dx++) {
          ra += sp->r;
          ga += sp->g;
          ba += sp->b;
          aa += sp->a;
          
          sp++;
         } 
         /* src dx loop */
         sp = (tColorRGBA *)((Uint8*)sp + (src->pitch - 4*factorx)); // next y
        }
        /* src dy loop */

        /* next box-x */
        sp = (tColorRGBA *)((Uint8*)oosp + 4*factorx);
                
        /* Store result in destination */
        dp->r = ra/n_average;
        dp->g = ga/n_average;
        dp->b = ba/n_average;
        dp->a = aa/n_average;
                 
        /*
         * Advance destination pointer 
         */
         dp++;
        } 
        /* dst x loop */

        /* next box-y */
        sp = (tColorRGBA *)((Uint8*)osp + src->pitch*factory);

        /*
         * Advance destination pointers 
         */
        dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
      } 
      /* dst y loop */

    return (0);
}

/* 
 
 8bit integer-factor averaging Shrinker

 Shrinks 8bit Y 'src' surface to 'dst' surface.
 
*/

int shrinkSurfaceY(SDL_Surface * src, SDL_Surface * dst, int factorx, int factory)
{
    int x, y, dx, dy, sgap, dgap, a;
    int n_average;
    Uint8 *sp, *osp, *oosp;
    Uint8 *dp;

    /*
     * Averaging integer shrink
     */

    /* Precalculate division factor */
    n_average = factorx*factory;
   
    /*
     * Scan destination
     */
    sp = (Uint8 *) src->pixels;
    sgap = src->pitch - src->w;

    dp = (Uint8 *) dst->pixels;
    dgap = dst->pitch - dst->w;
    
    for (y = 0; y < dst->h; y++) {    

      osp=sp;
      for (x = 0; x < dst->w; x++) {

        /* Trace out source box and accumulate */
        oosp=sp;
        a=0;
        for (dy=0; dy < factory; dy++) {
         for (dx=0; dx < factorx; dx++) {
          a += (*sp);
          /* next x */           
          sp++;
         } 
         /* end src dx loop */         
         /* next y */
         sp = (Uint8 *)((Uint8*)sp + (src->pitch - factorx)); 
        } 
        /* end src dy loop */
        
        /* next box-x */
        sp = (Uint8 *)((Uint8*)oosp + factorx);
                
        /* Store result in destination */
        *dp = a/n_average;

        /*
         * Advance destination pointer 
         */
         dp++;
        } 
        /* end dst x loop */

        /* next box-y */
        sp = (Uint8 *)((Uint8*)osp + src->pitch*factory);

        /*
         * Advance destination pointers 
         */
        dp = (Uint8 *)((Uint8 *)dp + dgap);
      } 
      /* end dst y loop */

    return (0);
}

/* 
 
 32bit Zoomer with optional anti-aliasing by bilinear interpolation.

 Zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

int zoomSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy, int smooth)
{
    int x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy, ex, ey, t1, t2, sstep, lx, ly;
    tColorRGBA *c00, *c01, *c10, *c11, *cswap;
    tColorRGBA *sp, *csp, *dp;
    int dgap;

    /*
     * Variable setup 
     */
    if (smooth) {
	/*
	 * For interpolation: assume source dimension is one pixel 
	 */
	/*
	 * smaller to avoid overflow on right and bottom edge.     
	 */
	sx = (int) (65536.0 * (float) (src->w - 1) / (float) dst->w);
	sy = (int) (65536.0 * (float) (src->h - 1) / (float) dst->h);
    } else {
	sx = (int) (65536.0 * (float) src->w / (float) dst->w);
	sy = (int) (65536.0 * (float) src->h / (float) dst->h);
    }

    /*
     * Allocate memory for row increments 
     */
    if ((sax = (int *) malloc((dst->w + 1) * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (int *) malloc((dst->h + 1) * sizeof(Uint32))) == NULL) {
	free(sax);
	return (-1);
    }

    /*
     * Precalculate row increments 
     */
    sp = csp = (tColorRGBA *) src->pixels;
    dp = (tColorRGBA *) dst->pixels;

    if (flipx) csp += (src->w-1);
    if (flipy) csp += (src->pitch*(src->h-1));

    csx = 0;
    csax = sax;
    for (x = 0; x <= dst->w; x++) {
	*csax = csx;
	csax++;
	csx &= 0xffff;
	csx += sx;
    }
    csy = 0;
    csay = say;
    for (y = 0; y <= dst->h; y++) {
	*csay = csy;
	csay++;
	csy &= 0xffff;
	csy += sy;
    }

    dgap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {

	/*
	 * Interpolating Zoom 
	 */

	/*
	 * Scan destination 
	 */
	ly = 0;
	csay = say;
	for (y = 0; y < dst->h; y++) {
            /*
             * Setup color source pointers 
             */
            c00 = csp;	    
            c01 = csp;
            c01++;	    
            c10 = (tColorRGBA *) ((Uint8 *) csp + src->pitch);
            c11 = c10;
            c11++;
            csax = sax;
            if (flipx) {
	     cswap = c00; c00=c01; c01=cswap;
	     cswap = c10; c10=c11; c11=cswap;
            }
            if (flipy) {
	     cswap = c00; c00=c10; c10=cswap;
	     cswap = c01; c01=c11; c11=cswap;
            }
            lx = 0;
	    for (x = 0; x < dst->w; x++) {
		/*
		 * Interpolate colors 
		 */
		ex = (*csax & 0xffff);
		ey = (*csay & 0xffff);
		t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
		t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;
		dp->r = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
		t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
		dp->g = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
		t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
		dp->b = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
		t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
		dp->a = (((t2 - t1) * ey) >> 16) + t1;

		/*
		 * Advance source pointers 
		 */
		csax++;
		sstep = (*csax >> 16);
		lx += sstep;
		if (lx >= src->w) sstep = 0;
		if (flipx) sstep = -sstep;
		c00 += sstep;
		c01 += sstep;
		c10 += sstep;
		c11 += sstep;
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    sstep = (*csay >> 16);
            ly += sstep;
            if (ly >= src->h) sstep = 0;
            sstep *= src->pitch;
	    if (flipy) sstep = -sstep;
	    csp = (tColorRGBA *) ((Uint8 *) csp + sstep);

	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}
    } else {

	/*
	 * Non-Interpolating Zoom 
	 */

	csay = say;
	for (y = 0; y < dst->h; y++) {
	    sp = csp;
	    csax = sax;
	    for (x = 0; x < dst->w; x++) {
		/*
		 * Draw 
		 */
		*dp = *sp;
		/*
		 * Advance source pointers 
		 */
		csax++;
		sstep = (*csax >> 16);
		if (flipx) sstep = -sstep;
		sp += sstep;
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    sstep = (*csay >> 16) * src->pitch;
	    if (flipy) sstep = -sstep;
	    csp = (tColorRGBA *) ((Uint8 *) csp + sstep);

	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}
    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 8bit Zoomer without smoothing.

 Zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/

int zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst, int flipx, int flipy)
{
    Uint32 x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy, sstep;
    Uint8 *sp, *dp, *csp;
    int dgap;

    /*
     * Variable setup 
     */
    sx = (Uint32) (65536.0 * (float) src->w / (float) dst->w);
    sy = (Uint32) (65536.0 * (float) src->h / (float) dst->h);

 
     /*
     * Allocate memory for row increments 
     */
    if ((sax = (Uint32 *) malloc((dst->w + 1) * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (Uint32 *) malloc((dst->h + 1) * sizeof(Uint32))) == NULL) {
	free(sax);
	return (-1);
    }

    /*
     * Pointer setup 
     */
    sp = csp = (Uint8 *) src->pixels;
    dp = (Uint8 *) dst->pixels;
    dgap = dst->pitch - dst->w;

    if (flipx) csp += (src->w-1);
    if (flipy) csp  = ( (Uint8*)csp + src->pitch*(src->h-1) );

    /*
     * Precalculate row increments 
     */
    csx = 0;
    csax = sax;
    for (x = 0; x <= dst->w; x++) {
	*csax = csx;
	csax++;
	csx &= 0xffff;
	csx += sx;
    }
    csy = 0;
    csay = say;
    for (y = 0; y <= dst->h; y++) {
	*csay = csy;
	csay++;
	csy &= 0xffff;
	csy += sy;
    }


    /*
     * Draw 
     */
    csay = say;
    for (y = 0; y < dst->h; y++) {
	csax = sax;
	sp = csp;
	for (x = 0; x < dst->w; x++) {
	    /*
	     * Draw 
	     */
	    *dp = *sp;
	    /*
	     * Advance source pointers 
	     */
	    csax++;
            sstep = (*csax >> 16);
            if (flipx) sstep = -sstep;
            sp += sstep;
	    /*
	     * Advance destination pointer 
	     */
	    dp++;
	}
	/*
	 * Advance source pointer (for row) 
	 */
	csay++;
        sstep = (*csay >> 16) * src->pitch;
        if (flipy) sstep = -sstep;
        csp = ((Uint8 *) csp + sstep);

	/*
	 * Advance destination pointers 
	 */
	dp += dgap;
    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 32bit Rotozoomer with optional anti-aliasing by bilinear interpolation.

 Rotates and zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

void transformSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, int flipx, int flipy, int smooth)
{
    int x, y, t1, t2, dx, dy, xd, yd, sdx, sdy, ax, ay, ex, ey, sw, sh;
    tColorRGBA c00, c01, c10, c11, cswap;
    tColorRGBA *pc, *sp;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
    pc = dst->pixels;
    gap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (sdx >> 16);
		dy = (sdy >> 16);
		if ((dx > -1) && (dy > -1) && (dx < src->w) && (dy < src->h)) {
  		    if (flipx) dx = sw - dx;
  		    if (flipy) dy = sh - dy;
                    sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
                    sp += dx;
                    c00 = *sp;
                    sp += 1;
                    c01 = *sp;
                    sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
                    c11 = *sp;
                    sp -= 1;
                    c10 = *sp;
                    if (flipx) {
                      cswap = c00; c00=c01; c01=cswap;
                      cswap = c10; c10=c11; c11=cswap;
                    }
                    if (flipy) {
                      cswap = c00; c00=c10; c10=cswap;
                      cswap = c01; c01=c11; c11=cswap;
                    }
		    /*
		     * Interpolate colors 
		     */
		    ex = (sdx & 0xffff);
		    ey = (sdy & 0xffff);
		    t1 = ((((c01.r - c00.r) * ex) >> 16) + c00.r) & 0xff;
		    t2 = ((((c11.r - c10.r) * ex) >> 16) + c10.r) & 0xff;
		    pc->r = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.g - c00.g) * ex) >> 16) + c00.g) & 0xff;
		    t2 = ((((c11.g - c10.g) * ex) >> 16) + c10.g) & 0xff;
		    pc->g = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.b - c00.b) * ex) >> 16) + c00.b) & 0xff;
		    t2 = ((((c11.b - c10.b) * ex) >> 16) + c10.b) & 0xff;
		    pc->b = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.a - c00.a) * ex) >> 16) + c00.a) & 0xff;
		    t2 = ((((c11.a - c10.a) * ex) >> 16) + c10.a) & 0xff;
		    pc->a = (((t2 - t1) * ey) >> 16) + t1;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    } else {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (short) (sdx >> 16);
		dy = (short) (sdy >> 16);
		if (flipx) dx = (src->w-1)-dx;
		if (flipy) dy = (src->h-1)-dy;
		if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		    sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
		    sp += dx;
		    *pc = *sp;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    }
}

/* 
 
 8bit Rotozoomer without smoothing

 Rotates and zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/

void transformSurfaceY(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, int flipx, int flipy)
{
    int x, y, dx, dy, xd, yd, sdx, sdy, ax, ay, sw, sh;
    tColorY *pc, *sp;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
    pc = dst->pixels;
    gap = dst->pitch - dst->w;
    /*
     * Clear surface to colorkey 
     */
    memset(pc, (unsigned char) (src->format->colorkey & 0xff), dst->pitch * dst->h);
    /*
     * Iterate through destination surface 
     */
    for (y = 0; y < dst->h; y++) {
	dy = cy - y;
	sdx = (ax + (isin * dy)) + xd;
	sdy = (ay - (icos * dy)) + yd;
	for (x = 0; x < dst->w; x++) {
	    dx = (short) (sdx >> 16);
	    dy = (short) (sdy >> 16);
            if (flipx) dx = (src->w-1)-dx;
            if (flipy) dy = (src->h-1)-dy;
	    if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		sp = (tColorY *) (src->pixels);
		sp += (src->pitch * dy + dx);
		*pc = *sp;
	    }
	    sdx += icos;
	    sdy += isin;
	    pc++;
	}
	pc += gap;
    }
}

/* 
 
 32bit specialized 90degree rotator

 Rotates and zooms 'src' surface to 'dst' surface in 90degree increments.

 (contributed by Jeff Schiller)
 
*/
SDL_Surface* rotateSurface90Degrees(SDL_Surface* pSurf, int numClockwiseTurns) 
{
 int row, col, newWidth, newHeight;
 SDL_Surface* pSurfOut;
 
 /* Has to be a valid surface pointer and only 32-bit surfaces (for now) */
 if (!pSurf || pSurf->format->BitsPerPixel != 32) { return NULL; }

 /* normalize numClockwiseTurns */
 while(numClockwiseTurns < 0) { numClockwiseTurns += 4; }
 numClockwiseTurns = (numClockwiseTurns % 4);

 /* if it's even, our new width will be the same as the source surface */
 newWidth = (numClockwiseTurns % 2) ? (pSurf->h) : (pSurf->w);
 newHeight = (numClockwiseTurns % 2) ? (pSurf->w) : (pSurf->h);
 pSurfOut = SDL_CreateRGBSurface( pSurf->flags, newWidth, newHeight, pSurf->format->BitsPerPixel,
                           pSurf->format->Rmask,
                           pSurf->format->Gmask, 
                           pSurf->format->Bmask, 
                           pSurf->format->Amask);
 if(!pSurfOut) {
   return NULL;
 }

 if(numClockwiseTurns != 0) {
   SDL_LockSurface(pSurf);
   SDL_LockSurface(pSurfOut);
   switch(numClockwiseTurns) {
     
     /* rotate clockwise */
     case 1: /* rotated 90 degrees clockwise */
     {
       Uint32* srcBuf = NULL;
       Uint32* dstBuf = NULL;

       for (row = 0; row < pSurf->h; ++row) {
         srcBuf = (Uint32*)(pSurf->pixels) + (row*pSurf->pitch/4);
         dstBuf = (Uint32*)(pSurfOut->pixels) + (pSurfOut->w - row - 1);
         for (col = 0; col < pSurf->w; ++col) {
           *dstBuf = *srcBuf;
           ++srcBuf;
           dstBuf += pSurfOut->pitch/4;
         } 
         /* end for(col) */
       } 
       /* end for(row) */
     }
     break;

     case 2: /* rotated 180 degrees clockwise */
     {
       Uint32* srcBuf = NULL;
       Uint32* dstBuf = NULL;

       for(row = 0; row < pSurf->h; ++row) {
         srcBuf = (Uint32*)(pSurf->pixels) + (row*pSurf->pitch/4);
         dstBuf = (Uint32*)(pSurfOut->pixels) + ((pSurfOut->h - row - 1)*pSurfOut->pitch/4) + (pSurfOut->w - 1);
         for(col = 0; col < pSurf->w; ++col) {
           *dstBuf = *srcBuf;
           ++srcBuf;
           --dstBuf;
         } 
       } 
     }
     break;

     case 3:
     {
       Uint32* srcBuf = NULL;
       Uint32* dstBuf = NULL;

       for(row = 0; row < pSurf->h; ++row) {
         srcBuf = (Uint32*)(pSurf->pixels) + (row*pSurf->pitch/4);
         dstBuf = (Uint32*)(pSurfOut->pixels) + row + ((pSurfOut->h - 1)*pSurfOut->pitch/4);
         for(col = 0; col < pSurf->w; ++col) {
           *dstBuf = *srcBuf;
           ++srcBuf;
           dstBuf -= pSurfOut->pitch/4;
         } 
       } 
     }
     break;
   } 
   /* end switch */

   SDL_UnlockSurface(pSurf);
   SDL_UnlockSurface(pSurfOut);
 } 
 /* end if numClockwiseTurns > 0 */
 else {
   /* simply copy surface to output */
   if(SDL_BlitSurface(pSurf, NULL, pSurfOut, NULL)) {
     return NULL;
   }
 }
 return pSurfOut;
}

/* 
 
 rotozoomSurface()

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoom' a scaling factor. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001

/* Local rotozoom-size function with trig result return */

void rotozoomSurfaceSizeTrig(int width, int height, double angle, double zoomx, double zoomy, int *dstwidth, int *dstheight, 
			     double *canglezoom, double *sanglezoom)
{
    double x, y, cx, cy, sx, sy;
    double radangle;
    int dstwidthhalf, dstheighthalf;

    /*
     * Determine destination width and height by rotating a centered source box 
     */
    radangle = angle * (M_PI / 180.0);
    *sanglezoom = sin(radangle);
    *canglezoom = cos(radangle);
    *sanglezoom *= zoomx;
    *canglezoom *= zoomx;
    x = width / 2;
    y = height / 2;
    cx = *canglezoom * x;
    cy = *canglezoom * y;
    sx = *sanglezoom * x;
    sy = *sanglezoom * y;
    
    dstwidthhalf = MAX((int)
		       ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
    dstheighthalf = MAX((int)
			ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);
    *dstwidth = 2 * dstwidthhalf;
    *dstheight = 2 * dstheighthalf;
}


/* Publically available rotozoom-size function */

void rotozoomSurfaceSizeXY(int width, int height, double angle, double zoomx, double zoomy, int *dstwidth, int *dstheight)
{
    double dummy_sanglezoom, dummy_canglezoom;

    rotozoomSurfaceSizeTrig(width, height, angle, zoomx, zoomy, dstwidth, dstheight, &dummy_sanglezoom, &dummy_canglezoom);
}

/* Publically available rotozoom-size function */

void rotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight)
{
    double dummy_sanglezoom, dummy_canglezoom;

    rotozoomSurfaceSizeTrig(width, height, angle, zoom, zoom, dstwidth, dstheight, &dummy_sanglezoom, &dummy_canglezoom);
}

/* Publically available rotozoom function */

SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, double zoom, int smooth)
{
  return rotozoomSurfaceXY(src, angle, zoom, zoom, smooth);
}

/* Publically available rotozoom function */

SDL_Surface *rotozoomSurfaceXY(SDL_Surface * src, double angle, double zoomx, double zoomy, int smooth)
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    double zoominv;
    double sanglezoom, canglezoom, sanglezoominv, canglezoominv;
    int dstwidthhalf, dstwidth, dstheighthalf, dstheight;
    int is32bit;
    int i, src_converted;
    int flipx,flipy;
    Uint8 r,g,b;
    Uint32 colorkey;
    int colorKeyAvailable = 0;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    if( src->flags & SDL_SRCCOLORKEY )
    {
        colorkey = src->format->colorkey;
        SDL_GetRGB(colorkey, src->format, &r, &g, &b);
        colorKeyAvailable = 1;
    }
    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit)/* || (src->format->BitsPerPixel == 8)*/) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#else
                                0xff000000,  0x00ff0000, 0x0000ff00, 0x000000ff
#endif
	    );
		if(colorKeyAvailable)
			SDL_SetColorKey(src, 0, 0);

	SDL_BlitSurface(src, NULL, rz_src, NULL);

		if(colorKeyAvailable)
			SDL_SetColorKey(src, SDL_SRCCOLORKEY, colorkey);
	src_converted = 1;
	is32bit = 1;
    }

    /*
     * Sanity check zoom factor 
     */
    flipx = (zoomx<0.0);
    if (flipx) zoomx=-zoomx;
    flipy = (zoomy<0.0);
    if (flipy) zoomy=-zoomy;
    if (zoomx < VALUE_LIMIT) zoomx = VALUE_LIMIT;
    if (zoomy < VALUE_LIMIT) zoomy = VALUE_LIMIT;
    zoominv = 65536.0 / (zoomx * zoomx);

    /*
     * Check if we have a rotozoom or just a zoom 
     */
    if (fabs(angle) > VALUE_LIMIT) {

	/*
	 * Angle!=0: full rotozoom 
	 */
	/*
	 * ----------------------- 
	 */

	/* Determine target size */
	rotozoomSurfaceSizeTrig(rz_src->w, rz_src->h, angle, zoomx, zoomy, &dstwidth, &dstheight, &canglezoom, &sanglezoom);

	/*
	 * Calculate target factors from sin/cos and zoom 
	 */
	sanglezoominv = sanglezoom;
	canglezoominv = canglezoom;
	sanglezoominv *= zoominv;
	canglezoominv *= zoominv;

	/* Calculate half size */
	dstwidthhalf = dstwidth / 2;
	dstheighthalf = dstheight / 2;

	/*
	 * Alloc space to completely contain the rotated surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	if (colorKeyAvailable == 1){
		colorkey = SDL_MapRGB(rz_dst->format, r, g, b);
        
		SDL_FillRect(rz_dst, NULL, colorkey );
	}
	
	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the rotation (using alpha) 
	     */
	    transformSurfaceRGBA(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
				 (int) (sanglezoominv), (int) (canglezoominv), 
				 flipx, flipy,
				 smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the rotation 
	     */
	    transformSurfaceY(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
			      (int) (sanglezoominv), (int) (canglezoominv),
			      flipx, flipy);
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);

    } else {

	/*
	 * Angle=0: Just a zoom 
	 */
	/*
	 * -------------------- 
	 */

	/*
	 * Calculate target size
	 */
	zoomSurfaceSize(rz_src->w, rz_src->h, zoomx, zoomy, &dstwidth, &dstheight);

	/*
	 * Alloc space to completely contain the zoomed surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	if (colorKeyAvailable == 1){
		colorkey = SDL_MapRGB(rz_dst->format, r, g, b);
        
		SDL_FillRect(rz_dst, NULL, colorkey );
	}

	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the zooming (using alpha) 
	     */
	    zoomSurfaceRGBA(rz_src, rz_dst, flipx, flipy, smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the zooming 
	     */
	    zoomSurfaceY(rz_src, rz_dst, flipx, flipy);
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);
    }

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}

/* 
 
 zoomSurface()

 Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001

void zoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight)
{
    /*
     * Sanity check zoom factors 
     */
    if (zoomx < VALUE_LIMIT) {
	zoomx = VALUE_LIMIT;
    }
    if (zoomy < VALUE_LIMIT) {
	zoomy = VALUE_LIMIT;
    }

    /*
     * Calculate target size 
     */
    *dstwidth = (int) ((double) width * zoomx);
    *dstheight = (int) ((double) height * zoomy);
    if (*dstwidth < 1) {
	*dstwidth = 1;
    }
    if (*dstheight < 1) {
	*dstheight = 1;
    }
}

SDL_Surface *zoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth)
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    int dstwidth, dstheight;
    int is32bit;
    int i, src_converted;
    int flipx, flipy;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit) /*|| (src->format->BitsPerPixel == 8)*/) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#else
                                0xff000000,  0x00ff0000, 0x0000ff00, 0x000000ff
#endif
	    );
	SDL_BlitSurface(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    flipx = (zoomx<0.0);
    if (flipx) zoomx = -zoomx;
    flipy = (zoomy<0.0);
    if (flipy) zoomy = -zoomy;

    /* Get size if target */
    zoomSurfaceSize(rz_src->w, rz_src->h, zoomx, zoomy, &dstwidth, &dstheight);

    /*
     * Alloc space to completely contain the zoomed surface 
     */
    rz_dst = NULL;
    if (is32bit) {
	/*
	 * Target surface is 32bit with source RGBA/ABGR ordering 
	 */
	rz_dst =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				 rz_src->format->Rmask, rz_src->format->Gmask,
				 rz_src->format->Bmask, rz_src->format->Amask);
    } else {
	/*
	 * Target surface is 8bit 
	 */
	rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
    }

    /*
     * Lock source surface 
     */
    SDL_LockSurface(rz_src);
    /*
     * Check which kind of surface we have 
     */
    if (is32bit) {
	/*
	 * Call the 32bit transformation routine to do the zooming (using alpha) 
	 */
	zoomSurfaceRGBA(rz_src, rz_dst, flipx, flipy, smooth);
	/*
	 * Turn on source-alpha support 
	 */
	SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
    } else {
	/*
	 * Copy palette and colorkey info 
	 */
	for (i = 0; i < rz_src->format->palette->ncolors; i++) {
	    rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	}
	rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	/*
	 * Call the 8bit transformation routine to do the zooming 
	 */
	zoomSurfaceY(rz_src, rz_dst, flipx, flipy);
	SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
    }
    /*
     * Unlock source surface 
     */
    SDL_UnlockSurface(rz_src);

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}

SDL_Surface *shrinkSurface(SDL_Surface * src, int factorx, int factory)
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    int dstwidth, dstheight;
    int is32bit;
    int i, src_converted;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit) /*|| (src->format->BitsPerPixel == 8)*/) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#else
                                0xff000000,  0x00ff0000, 0x0000ff00, 0x000000ff
#endif
	    );
	SDL_BlitSurface(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    /* Get size for target */
    dstwidth=rz_src->w/factorx;
    while (dstwidth*factorx>rz_src->w) { dstwidth--; }
    dstheight=rz_src->h/factory;
    while (dstheight*factory>rz_src->h) { dstheight--; }

    /*
     * Alloc space to completely contain the shrunken surface 
     */
    rz_dst = NULL;
    if (is32bit) {
	/*
	 * Target surface is 32bit with source RGBA/ABGR ordering 
	 */
	rz_dst =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				 rz_src->format->Rmask, rz_src->format->Gmask,
				 rz_src->format->Bmask, rz_src->format->Amask);
    } else {
	/*
	 * Target surface is 8bit 
	 */
	rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
    }

    /*
     * Lock source surface 
     */
    SDL_LockSurface(rz_src);
    /*
     * Check which kind of surface we have 
     */
    if (is32bit) {
	/*
	 * Call the 32bit transformation routine to do the shrinking (using alpha) 
	 */
	shrinkSurfaceRGBA(rz_src, rz_dst, factorx, factory);
	/*
	 * Turn on source-alpha support 
	 */
	SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
    } else {
	/*
	 * Copy palette and colorkey info 
	 */
	for (i = 0; i < rz_src->format->palette->ncolors; i++) {
	    rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	}
	rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	/*
	 * Call the 8bit transformation routine to do the shrinking 
	 */
	shrinkSurfaceY(rz_src, rz_dst, factorx, factory);
	SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
    }
    /*
     * Unlock source surface 
     */
    SDL_UnlockSurface(rz_src);

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}
