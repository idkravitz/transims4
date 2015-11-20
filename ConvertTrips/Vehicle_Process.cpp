//*********************************************************
//	Vehicle_Process.cpp - copy the vehicle file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool ConvertTrips::Vehicle_Processing (Db_File *file)
{
	int hhold, veh;

	vehicle_file->Copy_Fields ((Vehicle_File *) file);

	hhold = vehicle_file->Household ();
	veh = vehicle_file->Vehicle ();

	if (hhold > max_hh_in) max_hh_in = hhold;
	if (veh > max_veh_in) max_veh_in = veh;

	if (!vehicle_file->Write ()) {
		Error ("Writing Vehicle File");
	}
	veh_copy++;

	//---- don't save the record ----

	return (false);
}
