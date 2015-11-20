//*********************************************************
//	Household_Process.cpp - Read the Household File
//*********************************************************

#include "ModeChoice.hpp"

#include "Household_File.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool ModeChoice::Household_Processing (Db_File *fh)
{
	Household_File *file = (Household_File *) fh;

	if (hhlist_flag) {
		if (First_Record ()) {
			if (!household_db.Max_Records (hhold_list.Num_Records ())) goto mem_error;
		}
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	} else if (First_Record ()) {
		if (!household_db.Max_Records (file->Max_Record_Number ())) goto mem_error;
	}

	//---- copy the data for user programs ----

	household_db.Copy_Fields (file);

	if (!household_db.Add_Record ()) {
		Error ("Writing Household Database");
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Household Database");
	return (false);
}
