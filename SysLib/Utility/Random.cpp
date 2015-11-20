//********************************************************* 
//	Random.cpp - randome number class
//*********************************************************

#include "Random.hpp"

#include <time.h>

//-----------------------------------------------------------
//	Random constructor
//-----------------------------------------------------------

Random::Random (int seed) 
{
	Seed (seed);
}

//---------------------------------------------------------
//	random_seed - set the random number seed
//---------------------------------------------------------

int Random::Seed (int seed)
{
	if (seed < 1) {
		seed = (int) time (NULL);
	}
	next_random_number = seed;

	return (seed);
}

//---------------------------------------------------------
//	Probability - random probability
//---------------------------------------------------------

double Random::Probability (void)
{
	next_random_number = next_random_number * 1103515245L + 12345L;
	return ((double) ((unsigned int) (next_random_number >> 16) & 0x7fff) / (0x7fff + 1.0));
}
