//*********************************************************
//	Population_Process.cpp - Read the Population_File
//*********************************************************

#include "PopSyn.hpp"

#include "Population_File.hpp"

//---------------------------------------------------------
//	Population_Processing
//---------------------------------------------------------

bool PopSyn::Population_Processing (Db_File *fh)
{
	int hhold;
	Population_File *file;

	file = (Population_File *) fh;

	hhold = file->Household ();
	if (hhold == 0) return (false);

	old_pop++;

	new_pop_file->Copy_Fields (file);

	if (!new_pop_file->Write ()) {
		Error ("Writing %s", new_pop_file->File_Type ());
	}
	return (false);
}
