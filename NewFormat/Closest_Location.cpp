//*********************************************************
//	Closest_Location.cpp - Closest Activity Location
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Closest_Location
//---------------------------------------------------------

int NewFormat::Closest_Location (XY_Point *xy)
{
	int location, x, y;
	double dx, dy, closest, distance;

	Location_Data *loc_ptr;

	x = Round (xy->x);
	y = Round (xy->y);
	if (x == 0 && y == 0) return (0);

	closest = 0.0;
	location = 0;

	for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
		if (loc_ptr->Zone () == 0) continue;
		dx = loc_ptr->X () - x;
		dy = loc_ptr->Y () - y;

		distance = dx * dx + dy * dy;

		if (location == 0 || distance < closest) {
			location = loc_ptr->Location ();
			closest = distance;
		}
	}
	return (location);
}

