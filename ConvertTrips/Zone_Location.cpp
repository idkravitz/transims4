//*********************************************************
//	Zone_Location.cpp - check the zone data
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Zone_Location
//---------------------------------------------------------

void ConvertTrips::Zone_Location (void)
{
	int i, count, *zone;

	Convert_Location *loc_ptr;
	Zone_Data *zone_ptr;

	if (num_zone == 0) {
		Error ("No Zones were found in the Activity Location File");
	}
	Print (2, "Highest Traffic Analysis Zone = %d", num_zone);

	//---- allocate zone based memory ----

	if (!zone_loc.Num_Records (num_zone)) {
		Error ("Insufficient Memory to Load Zone-Based Data");
	}

	//---- process the location data ----

	for (loc_ptr =(Convert_Location *) location_data.First (); loc_ptr != NULL; 
			loc_ptr = (Convert_Location *) location_data.Next ()) {

		zone = (int *) zone_loc.Record (loc_ptr->Zone ());

		if (zone != NULL) {
			loc_ptr->Zone_List (*zone);
			*zone = location_data.Record_Index ();
		}
	}

	//---- search for locations without a zone number ----

	count = 0;

	for (i=1; i <= num_zone; i++) {
		if (zone_loc [i] != 0) continue;
		if (zone_loc_flag && zone_loc_map.Locations (i) != 0) continue;

		if (zone_flag) {
			zone_ptr = zone_data.Get (i);
			if (zone_ptr == NULL) continue;
		}
		if (count == 0) {
			Print (2, "No Trip End Location for Zones...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, "%d ", i);
	}

	//---- search for missing zone numbers ----

	if (zone_flag) {
		for (zone_ptr = zone_data.Get (num_zone+1); zone_ptr; zone_ptr = zone_data.Next_Key ()) {
			if (count == 0) {
				Print (2, "No Trip End Location for Zones...");
			}
			if (!(count % 10)) {
				Print (1, "\t");
			}
			count++;
			Print (0, "%d ", i);
		}
	}
	if (count) {
		Warning ("%d Zones have No Trip End Locations", count);
	}

	//---- search for locations with only one zone number ----

	count = 0;

	for (i=1; i <= num_zone; i++) {
		if (zone_loc [i] == 0) continue;

		if (zone_flag) {
			zone_ptr = zone_data.Get (i);
			if (zone_ptr == NULL) continue;
		}
		loc_ptr = (Convert_Location *) location_data [zone_loc [i]];

		if (loc_ptr->Zone_List () == 0) {
			if (zone_loc_flag && zone_loc_map.Locations (i) != 0) continue;

			if (count == 0) {
				Print (2, "Only One Trip End Location for Zones...");
			}
			if (!(count % 10)) {
				Print (1, "\t");
			}
			count++;
			Print (0, "%d ", i);
		}
	}
	if (count) {
		Warning ("%d Zones have only One Trip End Location", count);
	}

	if (!zone_flag) return;

	//---- search for zones not in the zone file ----

	count = 0;

	for (i=1; i <= num_zone; i++) {
		if (zone_loc [i] == 0) continue;

		zone_ptr = zone_data.Get (i);
		if (zone_ptr != NULL) continue;

		if (count == 0) {
			Print (2, "Zone Numbers Not in the Zone File...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, "%d ", i);
	}
	if (count) {
		Warning ("%d Zones are Not in the Zone File", count);
	}
}
