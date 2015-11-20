//*********************************************************
//	Random.hpp - random number class
//*********************************************************

#ifndef RANDOM_HPP
#define RANDOM_HPP

//---------------------------------------------------------
//	Random Class definition
//---------------------------------------------------------

class  Random
{
public:
	Random (int seed = 0);

	int Seed (int seed);
	int Seed (void)				{ return ((int) next_random_number); }

	int Percent (void)			{ return ((int) (Probability () * 100.0 + 0.5)); }

	double Probability (void);

private:
	unsigned int next_random_number;
};
#endif
