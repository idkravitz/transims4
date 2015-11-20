//*********************************************************
//	Closest_Zone.cpp - Find the Closest Zone Centroid
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Closest_Zone
//---------------------------------------------------------

int TransimsNet::Closest_Zone (int nx, int ny)
{
	int zone;
	double dx, dy, closest, distance;

	Zone_Data *zone_ptr;

	zone = 0;

	for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
		if (ext_zone > 0 && zone_ptr->Zone () >= ext_zone) continue;

		dx = zone_ptr->X () - nx;
		dy = zone_ptr->Y () - ny;

		distance = dx * dx + dy * dy;

		if (zone == 0 || distance < closest) {
			zone = zone_ptr->Zone ();
			closest = distance;
		}
	}
	return (zone);
}

