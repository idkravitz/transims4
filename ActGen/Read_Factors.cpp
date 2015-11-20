//*********************************************************
//	Read_Factors.cpp - read the balancing factor file
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Read_Factors
//---------------------------------------------------------

void ActGen::Read_Factors (void)
{
	int zone, zone_field, max_zone;
	double factor;

	Zone_Data *zone_ptr;
	Location_Data *loc_ptr;
	Purpose_Group *group_ptr;

	Show_Message ("Reading %s -- Record", balance_file.File_Type ());
	Set_Progress ();

	zone_field = balance_file.Field_Number ("ZONE");
	zone_ptr = zone_data.Last ();

	if (zone_ptr == NULL) {
		max_zone = 0;
		for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
			if (loc_ptr->Zone () > max_zone) max_zone = loc_ptr->Zone ();
		}
	} else {
		max_zone = zone_ptr->Zone ();
	}

	//---- initialize the zone data ----

	for (group_ptr = purpose_group.First (); group_ptr; group_ptr = purpose_group.Next ()) {
		if (group_ptr->Balance_Field () > 0) {
			group_ptr->Max_Zone (max_zone);
		}
	}

	//---- store the factors ----

	while (balance_file.Read ()) {
		Show_Progress ();

		balance_file.Get_Field (zone_field, &zone);
		if (zone <= 1  || zone > max_zone) continue;

		for (group_ptr = purpose_group.First (); group_ptr; group_ptr = purpose_group.Next ()) {
			if (group_ptr->Balance_Field () > 0) {
				balance_file.Get_Field (group_ptr->Balance_Field (), &factor);
				group_ptr->Balance_Factor (zone, factor);
			}
		}
	}
	End_Progress ();

	balance_file.Close ();
}
