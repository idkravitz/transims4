//*********************************************************
//	Household_Process.cpp - copy the household file
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool ConvertTours::Household_Processing (Db_File *file)
{
	int hhold;

	household_file->Copy_Fields ((Household_File *) file);

	hhold = household_file->Household ();

	if (hhold > max_hh_in) max_hh_in = hhold;

	if (!household_file->Write ()) {
		Error ("Writing Household File");
	}
	hhold_copy++;

	//---- don't save the record ----

	return (false);
}
