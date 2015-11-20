//*********************************************************
//	Read_Locations.cpp - read nearest zone location file
//*********************************************************

#include "PopSyn.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Zone_Locations
//---------------------------------------------------------

void PopSyn::Read_Zone_Locations (void)
{
	int  i, zone, location;
	char *str_ptr, buffer [FIELD_BUFFER];
	bool first, field;

	Zone_Location *zone_ptr = NULL;
	Location_List *list_ptr, *loc_ptr;

	//---- read the nearest zone location file ----

	Show_Message ("Reading %s -- Record", zone_loc_file.File_Type ());
	Set_Progress (100);

	first = true;
	
	while (zone_loc_file.Read ()) {
		Show_Progress ();

		str_ptr = zone_loc_file.Record ();

		//---- check for a header record ----

		if (first) {
			first = false;
			if (*str_ptr < '0' || *str_ptr > '9') continue;
		}

		//---- process each field ----

		field = true;

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			if (buffer [0] == '\0') break;

			if (field) {
				field = false;

				zone = atoi (buffer);
				if (zone == 0) break;

				zone_ptr = zone_location.Get (zone);

				if (zone_ptr == NULL) {
					Zone_Location zone_rec;

					zone_rec.Zone (zone);
					zone_rec.Location (0);

					if (!zone_location.Add (&zone_rec)) {
						Error ("Adding Zone Location Data");
					}
					zone_ptr = zone_location.Record ();
				}

			} else {

				location = atol (buffer);

				//---- get the existing location data ----

				loc_ptr = location_list.Get (location);

				if (loc_ptr == NULL) {
					Write (1, "Zone %d Location %d is Not in the Activity Location File", zone, location);
					continue;
				}

				//---- create a new location record ----

				list_ptr = location_list.New_List (location);

				if (list_ptr == NULL) {
					Error ("Insufficient Memory for the Location List");
				}

				//---- add the location to the zone list ----

				list_ptr->List (zone_ptr->Location ());
				zone_ptr->Location (location_list.Num_Records ());

				//---- copy the location data ----

				list_ptr->Parking (loc_ptr->Parking ());

				for (i=1; i <= location_list.Num_Weight (); i++) {
					list_ptr->Weight (i, loc_ptr->Weight (i));
				}
			}
		}
	}
	End_Progress ();
	zone_loc_file.Close ();

	Print (2, "Number of %s Records = %d", zone_loc_file.File_Type (), Progress_Count ());
}
