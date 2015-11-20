//*********************************************************
//	Read_Targets.cpp - read the balancing target file
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	Read_Targets
//---------------------------------------------------------

void ActBalance::Read_Targets (void)
{
	int zone, zone_field;
	double factor;

	Location_Data *loc_ptr;
	Factor_Group *group;

	//---- initialize the zone arrays ----

	max_zone = 0;
	for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
		if (loc_ptr->Zone () > max_zone) max_zone = loc_ptr->Zone ();
	
	}
	zone = max_zone + 1;

	for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
		group->target = new double [zone];
		group->current = new double [zone];

		if (group->target == NULL || group->current == NULL) {
			Error ("Insuffient Memory for %d Zone Records", max_zone);
		}
		memset (group->target, '\0', zone * sizeof (double));
		memset (group->current, '\0', zone * sizeof (double));
	}

	//---- read the target data ----

	Show_Message ("Reading %s -- Record", target_file.File_Type ());
	Set_Progress ();

	zone_field = target_file.Field_Number ("ZONE");

	while (target_file.Read ()) {
		Show_Progress ();

		target_file.Get_Field (zone_field, &zone);
		if (zone <= 1  || zone > max_zone) continue;

		for (group = (Factor_Group *) factor_group.First (); group; group = (Factor_Group *) factor_group.Next ()) {
			target_file.Get_Field (group->target_field, &factor);
			group->target [zone] = factor;
		}
	}
	End_Progress ();

	target_file.Close ();
}
