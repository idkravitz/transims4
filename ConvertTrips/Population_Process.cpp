//*********************************************************
//	Population_Process.cpp - copy the population file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Population_Processing
//---------------------------------------------------------

bool ConvertTrips::Population_Processing (Db_File *file)
{
	int hhold;

	population_file->Copy_Fields ((Population_File *) file);

	hhold = population_file->Household ();

	if (hhold > max_hh_in) max_hh_in = hhold;

	if (!population_file->Write ()) {
		Error ("Writing Population File");
	}
	pop_copy++;

	//---- don't save the record ----

	return (false);
}
