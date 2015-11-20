//*************************************************** 
//	Projection_Code.hpp - Coordinate Projection Codes
//***************************************************

#ifndef PROJECTION_CODE_HPP
#define PROJECTION_CODE_HPP

//---------------------------------------------------------
//	Projection_Code Class definition
//---------------------------------------------------------

class Projection_Code
{
public:
	Projection_Code (void)	{}

	enum Projection_Type {NO_PROJECTION, LATLONG, STATEPLANE, UTM};
	enum Projection_Units {NO_UNITS, DEGREES, FEET, METERS, MILES, MILLION_DEG, KILOMETERS};
};
#endif
