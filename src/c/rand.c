#include "rand.h"

/*
 * Pseudo-Random Number Generator (PRNG)
 * Linear Congruential Generator (LCG) is a classic, simple PRNG.
 * The formula in this rand() function is based on LCG.
 *
 * LCG is defined by recurrence relation: Xₙ₊₁ = (a * Xₙ + c) mod m
 * - Xₙ is the current seed (state)
 * - a is the multiplier 
 * - c is the increment
 * - m is the modulus
 *
 * glibc uses the following values for each:
 * - a = 1103515245
 * - c = 12345
 * - m = 2^32
 *
 * In this case, we don't need to define m since the
 * modulus is implicit due to 32-bit integer overflow.
 *
 * Regardless, this is how we will define our random formula.
 *
 */

#define a 1103515245
#define c 12345

static unsigned int seed = 1;

void srand(unsigned int s) { seed = s; }

int rand() {
	seed = seed * a + c;
	return (seed >> 16) & 0x7FFF;
}

int randRange(int min, int max) { return min + rand() % (max - min + 1); }
