//*********************************************************
//	Population_Process.cpp - Read the Population File
//*********************************************************

#include "ModeChoice.hpp"

#include "Population_File.hpp"

//---------------------------------------------------------
//	Population_Processing
//---------------------------------------------------------

bool ModeChoice::Population_Processing (Db_File *fh)
{
	Population_File *file = (Population_File *) fh;
	Population_Data pop_rec;

	if (hhlist_flag) {
		if (First_Record ()) {
			if (!person_db.Max_Records (hhold_list.Num_Records () * 5)) goto mem_error;
		}
		if (hhold_list.Get_Index (file->Household ()) == 0) return (false);
	} else if (First_Record ()) {
		if (!person_db.Max_Records (file->Max_Record_Number ())) goto mem_error;
	}

	//---- copy the data for user programs ----

	person_db.Copy_Fields (file);

	//---- add the record key ----

	pop_rec.Household (file->Household ());
	pop_rec.Person (file->Person ());

	person_db.Put_Field (1, pop_rec.ID ());

	if (!person_db.Add_Record ()) {
		Error ("Writing Person Database");
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for Person Database");
	return (false);
}
