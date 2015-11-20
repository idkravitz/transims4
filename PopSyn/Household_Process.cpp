//*********************************************************
//	Household_Process.cpp - Read the Household File
//*********************************************************

#include "PopSyn.hpp"

#include "Household_File.hpp"

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool PopSyn::Household_Processing (Db_File *fh)
{
	int hhold;
	Household_File *file;

	file = (Household_File *) fh;

	hhold = file->Household ();
	if (hhold == 0) return (false);

	if (hhold > max_hh) max_hh = hhold;
	old_hh++;

	new_hh_file->Copy_Fields (file);

	if (!new_hh_file->Write ()) {
		Error ("Writing %s", new_hh_file->File_Type ());
	}
	return (false);
}
