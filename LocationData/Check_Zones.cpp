//*********************************************************
//	Check_Zones.cpp - check the zone data
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Check_Zones
//---------------------------------------------------------

void LocationData::Check_Zones (void)
{
	int z, count, *zone, high_zone;

	Zone_Map *map_ptr;

	Show_Message (2, "Checking Zone Coverage");

	//---- get the highest zone number ----

	high_zone = 0;

	for (map_ptr = (Zone_Map *) zone_map.First (); map_ptr; map_ptr = (Zone_Map *) zone_map.Next ()) {
		if (map_ptr->zone > high_zone) {
			high_zone = map_ptr->zone;
		}
	}
	if (high_zone == 0) {
		Warning ("No Zones were found in the Activity Location File");
		return;
	}
	Print (2, "Highest Traffic Analysis Zone = %d", high_zone);

	//---- allocate zone memory ----

	zone = new int [high_zone + 1];
	if (zone == NULL) {
		Error ("Insufficient Memory to Zone Data");
	}
	memset (zone, '\0', high_zone * sizeof (int));

	//---- process the location data ----

	for (map_ptr = (Zone_Map *) zone_map.First (); map_ptr; map_ptr = (Zone_Map *) zone_map.Next ()) {
		if (map_ptr->zone >= 0) {
			zone [map_ptr->zone]++;
		}
	}

	//---- search for locations without a zone number ----

	count = 0;

	for (z=1; z <= high_zone; z++) {
		if (zone [z] == 0) {
			if (count == 0) {
				Print (2, "No Activity Locations for Zones...");
			}
			if (!(count % 10)) {
				Print (1, "\t");
			}
			count++;
			Print (0, "%d ", z);
		}
	}
	if (count) {
		Warning ("%d Zones have No Activity Locations", count);
	}

	//---- search for locations with only one zone number ----

	count = 0;

	for (z=1; z <= high_zone; z++) {
		if (zone [z] != 1) continue;

		if (count == 0) {
			Print (2, "Only One Activity Location for Zones...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, "%d ", z);
	}
	if (count) {
		Warning ("%d Zones have only One Activity Location", count);
	}

	//---- location with a zone number of zero ----

	if (zone [0] > 0) {
		Print (1);
		Warning ("%d Activity Locations have No Zone Number", zone [0]);
	}
}
