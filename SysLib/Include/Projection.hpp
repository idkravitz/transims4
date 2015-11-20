//*************************************************** 
//	Projection.hpp - Coordinate Projection Class
//***************************************************

#ifndef PROJECTION_HPP
#define PROJECTION_HPP

#include "Projection_Data.hpp"
#include "Projection_Code.hpp"

#include "Utility.hpp"

//-------------------------------------
//	Projection Class definition
//-------------------------------------

class  Projection : public Projection_Code
{
public:
	Projection (void);
	Projection (Projection_Data input, Projection_Data output);

	bool Set_Projection (Projection_Data input, Projection_Data output);

	bool Convert (double *x, double *y);

	bool Convert (XY_Point *pt)              { return (Convert (&(pt->x), &(pt->y))); }

	double Length_Factor (void);
	double Speed_Factor (void);

	Projection_Data * Input_Projection (void)  { return (&input); }
	Projection_Data * Output_Projection (void) { return (&output); }

private:

	void LatLongToUtm (Projection_Data *data, double *x, double *y);
	void UtmToLatLong (Projection_Data *data, double *x, double *y);
	void LatLongToSP (Projection_Data *data, double *x, double *y);
	void SPToLatLong (Projection_Data *data, double *x, double *y);

	Projection_Data input, output;
	bool status, convert_flag;
};
#endif
