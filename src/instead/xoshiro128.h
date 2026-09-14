#ifndef XOSHIRO128_H
#define XOSHIRO128_H
#ifdef _USE_SDL
#include <SDL3/SDL_stdinc.h>
#else
#include <stdint.h>
typedef uint32_t Uint32;
#endif
/**
 * @file xoshiro128.h
 *
 * @brief xoshiro128** pseudorandom number generator, 128 bit internal state
 *
 * Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 *
 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * See <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * Changes:
 * a) SDL3/SDL_stdinc.h Uint32 instead of uint32_t
 * b) static inline functions instead of a separate translation unit
 */

#if defined(PLAN9)
#define __inline
#endif

/**
 * xoshiro128** internal state vector
 */
struct XOSHIRO128_T {
	Uint32 status[4];
};

typedef struct XOSHIRO128_T xoshiro128_t;

void xoshiro128_init(xoshiro128_t * random, Uint32 seed);

/**
 * This function represents a circular left shift.
 * Users should not call this function directly.
 * @param x 32-bit integer
 * @param k shift count (0 < k < 32)
 * @return shifted 32-bit integer
 */
__inline static Uint32 xoshiro128_rotl(const Uint32 x, int k) {
	return (x << k) | (x >> (32 - k));
}

/**
 * This function changes internal state of xoshiro128** and outputs
 * 32-bit unsigned integer.
 * @param random xoshiro128 internal status
 * @return 32-bit unsigned pseudorandom number
 */
__inline static Uint32 xoshiro128_next(xoshiro128_t * random) {
	const Uint32 result = xoshiro128_rotl(random->status[1] * 5, 7) * 9;
	const Uint32 t = random->status[1] << 9;

	random->status[2] ^= random->status[0];
	random->status[3] ^= random->status[1];
	random->status[1] ^= random->status[2];
	random->status[0] ^= random->status[3];
	random->status[2] ^= t;
	random->status[3] = xoshiro128_rotl(random->status[3], 11);

	return result;
}

/**
 * This function outputs floating point number from internal state.
 * The returned value has 32-bit precision.
 * @param random xoshiro128 internal status
 * @return floating point number r (0.0 <= r < 1.0)
 */
__inline static double xoshiro128_double(xoshiro128_t * random) {
	return xoshiro128_next(random) * (1.0 / 4294967296.0);
}

#endif
