/**
 * @file xoshiro128.c
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
 */

#include "xoshiro128.h"

/**
 * This function represents the SplitMix32 generator. It is used only for
 * seeding (xoshiro128** state must not be initialized by a simple expansion
 * of a 32-bit seed).
 * @param x pointer to 32-bit integer (splitmix32 internal state)
 * @return 32-bit integer
 */
static Uint32 splitmix32(Uint32 * x) {
	Uint32 z = (*x += 0x9e3779b9u);
	z = (z ^ (z >> 16)) * 0x21f0aaadu;
	z = (z ^ (z >> 15)) * 0x735a2d97u;
	return z ^ (z >> 15);
}

/**
 * This function certificate the state is not the all-zero fixed point.
 * @param random xoshiro128 state vector.
 */
static void period_certification(xoshiro128_t * random) {
	if (random->status[0] == 0 && random->status[1] == 0 &&
	    random->status[2] == 0 && random->status[3] == 0) {
		random->status[0] = 'X';
		random->status[1] = 'O';
		random->status[2] = 'S';
		random->status[3] = 'H';
	}
}

/**
 * This function initializes the internal state array with a 32-bit
 * unsigned integer seed.
 * @param random xoshiro128 state vector.
 * @param seed a 32-bit unsigned integer used as a seed.
 */
void xoshiro128_init(xoshiro128_t * random, Uint32 seed) {
	int i;
	Uint32 x = seed;

	for (i = 0; i < 4; i++) {
		random->status[i] = splitmix32(&x);
	}
	period_certification(random);
}
