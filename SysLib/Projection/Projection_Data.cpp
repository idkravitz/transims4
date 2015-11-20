//*********************************************************
//	Projection_Data.cpp - Coordinate Projection Data
//*********************************************************

#include "Projection_Data.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Projection_Data constructor
//---------------------------------------------------------

Projection_Data::Projection_Data (void) : Projection_Code ()
{
	Type (NO_PROJECTION);
	Code (0);
	Units (NO_UNITS);
	X_Offset (0.0);
	Y_Offset (0.0);
	X_Factor (1.0);
	Y_Factor (1.0);

	long_origin = false_easting = false_northing = scale_factor = a = b = e = ef = e_square = 0.0;
	e_prime_sq = a1 = a2 = a3 = a4 = mo = e1 = m1 = m2 = t1 = t2 = tf = n = sf = rf = 0.0;
	adjust = status = false;
	index = 0;
}
