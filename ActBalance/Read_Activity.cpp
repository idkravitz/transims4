//*********************************************************
//	Read_Activity.cpp - process activity files
//*********************************************************

#include "ActBalance.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

bool ActBalance::Read_Activity (int file_num)
{
	int purpose, num, zone, num_used;

	Activity_File *act_file;
	Location_Data *loc_ptr;
	Factor_Group *group;

	act_file = (Activity_File *) Demand_Db_Base (ACTIVITY);

	if (file_num > 0) {
		if (!act_file->Open (file_num)) return (false);
	}

	//---- process each trip ----

	if (act_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", act_file->File_Type (), act_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", act_file->File_Type ());
	}
	Set_Progress (10000);
	num_used = 0;

	while (act_file->Read ()) {
		Show_Progress ();

		if (act_file->Household () == 0) continue;

		purpose = act_file->Purpose ();
		if (purpose < 1 || purpose >= MAX_PURPOSE) continue;

		num = purpose_map [purpose];
		if (num == 0) continue;

		group = (Factor_Group *) factor_group [num];
		if (group == NULL) continue;

		num = act_file->Location ();

		loc_ptr = location_data.Get (num);
		if (loc_ptr == NULL) continue;

		zone = loc_ptr->Zone ();
		if (zone < 1 || zone > max_zone) continue;

		group->current [zone] += 1.0;
		num_used++;
	}
	End_Progress ();

	num = Progress_Count ();
	total_act += num;
	total_used += num_used;

	if (act_file->Extend ()) {
		Print (2, "Number of Activity Records Read from %s = %d", act_file->Extension (), num);
	} else {
		Print (2, "Number of Activity Records Read = %d", num);
	}
	Print (1, "Number of Activity Records Used = %d", num_used);
	if (num > 0) {
		Print (0, " (%.1lf%%)", num_used * 100.0 / num);
	}
	return (true);
}
