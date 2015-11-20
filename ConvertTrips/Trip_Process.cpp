//*********************************************************
//	Trip_Process.cpp - copy the trip file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Trip_Processing
//---------------------------------------------------------

bool ConvertTrips::Trip_Processing (Db_File *file)
{
	int hhold, veh;

	trip_file->Copy_Fields ((Trip_File *) file);

	hhold = trip_file->Household ();
	veh = trip_file->Vehicle ();

	if (hhold > max_hh_in) max_hh_in = hhold;
	if (veh > max_veh_in) max_veh_in = veh;

	if (!trip_file->Write ()) {
		Error ("Writing Trip File");
	}
	trip_copy++;

	//---- don't save the record ----

	return (false);
}
