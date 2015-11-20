//*********************************************************
//	Closest.cpp - find the closest zone centroid
//*********************************************************

#include "Emissions.hpp"

//---------------------------------------------------------
//	Closest_Zone
//---------------------------------------------------------

int Emissions::Closest_Zone (int nx, int ny)
{
	int zone;
	double dx, dy, closest, distance;

	Zone_Data *zone_ptr;
	Location_Data *loc_ptr;

	zone = 0;

	if (zone_flag) {
		for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {
			dx = zone_ptr->X () - nx;
			dy = zone_ptr->Y () - ny;

			distance = dx * dx + dy * dy;

			if (zone == 0 || distance < closest) {
				zone = zone_ptr->Zone ();
				closest = distance;
			}
		}
	}
	if (location_flag) {
		for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
			if (loc_ptr->Zone () == 0) continue;

			dx = loc_ptr->X () - nx;
			dy = loc_ptr->Y () - ny;

			distance = dx * dx + dy * dy;

			if (zone == 0 || distance < closest) {
				zone = loc_ptr->Zone ();
				closest = distance;
			}
		}
	}
	return (zone);
}
