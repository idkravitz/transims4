//*********************************************************
//	Household_Process.cpp - Read the Household File
//*********************************************************

#include "ActivityPattern.hpp"

#include "Household_File.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool ActivityPattern::Household_Processing (Db_File *fh)
{
	int type;
	Type_Data *type_ptr;
	HHold_Data *household_ptr;
	Household_File *file;

	file = (Household_File *) fh;

	if (First_Record ()) {
		int max_rec;

		if (hhlist_flag) {
			max_rec = nhh_list;
		} else {
			max_rec = file->Estimate_Records ();
		}
		if (!household_data.Max_Records (max_rec)) goto mem_error;
	}
	if (hhlist_flag) {
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	}
	if (Demand_Service::Household_Processing (fh)) {
		household_ptr = household_data.New_Record ();

		household_ptr->Vehicle_List (0);
		household_ptr->Problem_Flag (false);

		if (hhold_type_flag) {
			household_ptr->Type (hhold_type.Execute ());
		}
		type = type_data.Get_Index (household_ptr->Type ());

		if (type == 0) {
			Error ("Household Type %d was Not Found in the Survey Data", household_ptr->Type ());
		}
		household_ptr->Type (type);

		type_ptr = type_data [type];

		type_ptr->Add_HHold ();

		return (true);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Household Database");
	return (false);
}
