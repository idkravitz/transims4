//*********************************************************
//	Household_Process.cpp - Read the Household File
//*********************************************************

#include "LocationChoice.hpp"

#include "Household_File.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool LocationChoice::Household_Processing (Db_File *fh)
{
	Household_Data *household_ptr;
	Household_File *file;

	file = (Household_File *) fh;

	if (First_Record ()) {
		int max_rec;

		if (hhlist_flag) {
			max_rec = hhold_list.Num_Records ();
		} else {
			max_rec = file->Estimate_Records ();
		}
		if (!household_data.Max_Records (max_rec)) goto mem_error;

		if (choice_flag) {
			if (!household_db.Max_Records (max_rec)) goto mem_error;
		}
	}
	if (hhlist_flag) {
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	}
	if (Demand_Service::Household_Processing (fh)) {
		household_ptr = household_data.New_Record ();
		household_ptr->Type (0);

		//---- copy the data for user programs ----

		if (choice_flag) {
			household_db.Copy_Fields (file);

			if (!household_db.Add_Record ()) {
				Error ("Writing Household Database");
			}
		}
		return (true);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Household Data");
	return (false);
}
