#ifndef TINYMT32_H
#define TINYMT32_H
#ifdef _USE_SDL
#include <SDL3/SDL_stdinc.h>
#else
#include <stdint.h>
typedef uint32_t Uint32;
#endif
/**
 * @file tinymt32.h
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * http://www.math.sci.hiroshima-u.ac.jp/~%20m-mat/MT/TINYMT/LICENSE.txt
 *
 * Changes:
 * a) some code removed
 * b) SDL_types.h Uint32 instead of uint32_t
 * c) url to LICENSE
 */

#if defined(PLAN9)
#define __inline
#endif

#if defined(PLAN9)
#define	UINT32_C(c)	(c ## U)
#else
#include <stdint.h>
#include <inttypes.h>
#endif

#define TINYMT32_MEXP 127
#define TINYMT32_SH0 1
#define TINYMT32_SH1 10
#define TINYMT32_SH8 8
#define TINYMT32_MASK UINT32_C(0x7fffffff)
#define TINYMT32_MUL (1.0f / 4294967296.0f)

/**
 * tinymt32 internal state vector and parameters
 */
struct TINYMT32_T {
    Uint32 status[4];
    Uint32 mat1;
    Uint32 mat2;
    Uint32 tmat;
};

typedef struct TINYMT32_T tinymt32_t;

void tinymt32_init(tinymt32_t * random, Uint32 seed);
void tinymt32_init_by_array(tinymt32_t * random, Uint32 init_key[],
			    int key_length);

/**
 * This function changes internal state of tinymt32.
 * Users should not call this function directly.
 * @param random tinymt internal status
 */
__inline static void tinymt32_next_state(tinymt32_t * random) {
    Uint32 x;
    Uint32 y;

    y = random->status[3];
    x = (random->status[0] & TINYMT32_MASK)
	^ random->status[1]
	^ random->status[2];
    x ^= (x << TINYMT32_SH0);
    y ^= (y >> TINYMT32_SH0) ^ x;
    random->status[0] = random->status[1];
    random->status[1] = random->status[2];
    random->status[2] = x ^ (y << TINYMT32_SH1);
    random->status[3] = y;
    random->status[1] ^= -((int32_t)(y & 1)) & random->mat1;
    random->status[2] ^= -((int32_t)(y & 1)) & random->mat2;
}

/**
 * This function outputs 32-bit unsigned integer from internal state.
 * Users should not call this function directly.
 * @param random tinymt internal status
 * @return 32-bit unsigned pseudorandom number
 */
__inline static Uint32 tinymt32_temper(tinymt32_t * random) {
    Uint32 t0, t1;
    t0 = random->status[3];
    t1 = random->status[0]
	+ (random->status[2] >> TINYMT32_SH8);
    t0 ^= t1;
    t0 ^= -((int32_t)(t1 & 1)) & random->tmat;
    return t0;
}

/**
 * This function outputs 32-bit unsigned integer from internal state.
 * @param random tinymt internal status
 * @return 32-bit unsigned integer r (0 <= r < 2^32)
 */
__inline static Uint32 tinymt32_generate_uint32(tinymt32_t * random) {
    tinymt32_next_state(random);
    return tinymt32_temper(random);
}

/**
 * This function outputs floating point number from internal state.
 * This function is implemented using multiplying by 1 / 2^32.
 * floating point multiplication is faster than using union trick in
 * my Intel CPU.
 * @param random tinymt internal status
 * @return floating point number r (0.0 <= r < 1.0)
 */
__inline static float tinymt32_generate_float(tinymt32_t * random) {
    tinymt32_next_state(random);
    return tinymt32_temper(random) * TINYMT32_MUL;
}

/**
 * This function outputs double precision floating point number from
 * internal state. The returned value has 32-bit precision.
 * In other words, this function makes one double precision floating point
 * number from one 32-bit unsigned integer.
 * @param random tinymt internal status
 * @return floating point number r (0.0 < r <= 1.0)
 */
__inline static double tinymt32_generate_32double(tinymt32_t * random) {
    tinymt32_next_state(random);
    return tinymt32_temper(random) * (1.0 / 4294967296.0);
}

#endif
