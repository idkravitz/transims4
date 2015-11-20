//*********************************************************
//	Exp.cpp - exponential function with limit warnings
//*********************************************************

#include "ActGen.hpp"

#include <math.h>

//---------------------------------------------------------
//	Exp function
//---------------------------------------------------------

double ActGen::Exp (double value)
{
	if (value < -708.0 || value > 709.0) {
		if (!warning_flag) {
			Write (1);
			Warning ("Exponetial Value %lf is Out of Range (-708..709)", value);
			warning_flag = true;
		}
		warning_count++;
		if (value > 709.0) {
			value = 709.0;
		} else {
			value = -708.0;
		}
	}
	return (exp (value));
}
