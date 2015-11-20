//*************************************************** 
//	Projection_Data.hpp - Coordinate Projection Data
//***************************************************

#ifndef PROJECTION_DATA_HPP
#define PROJECTION_DATA_HPP

#include "Projection_Code.hpp"

//---------------------------------------------------------
//	Projection_Data Class definition
//---------------------------------------------------------

class Projection_Data : public Projection_Code
{
public:
	Projection_Data (void);

	void Type (Projection_Type value)	{ type = value; }
	void Units (Projection_Units value)	{ units = value; }
	void Code (int value)				{ code = value; }
	void X_Offset (double value)		{ x_offset = value; }
	void Y_Offset (double value)		{ y_offset = value; }
	void X_Factor (double value)		{ x_factor = value; }
	void Y_Factor (double value)		{ y_factor = value; }
	void Status (bool flag)				{ status = flag; }

	Projection_Type  Type (void)		{ return (type); }
	Projection_Units Units (void)		{ return (units); }
	int              Code (void)		{ return (code); }
	double           X_Offset (void)	{ return (x_offset); }
	double           Y_Offset (void)	{ return (y_offset); }
	double           X_Factor (void)	{ return (x_factor); }
	double           Y_Factor (void)	{ return (y_factor); }
	bool             Status (void)		{ return (status); }

	double long_origin;
	double false_easting;
	double false_northing;
	double scale_factor;
	double a;
	double b;
	double e;
	double ef;
	double e_square;
	double e_prime_sq;
	double a1;
	double a2;
	double a3;
	double a4;
	double mo;
	double e1;
	double m1;
	double m2;
	double t1;
	double t2;
	double tf;
	double n;
	double sf;
	double rf;
	bool   adjust;
	int    index;

private:
	Projection_Type type;
	Projection_Units units;

	bool status;
	int code;
	double x_offset, y_offset, x_factor, y_factor;
};

#endif
