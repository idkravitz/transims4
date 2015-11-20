//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void PlanCompare::Read_Trips (Trip_File *file, Trip_Array *data)
{
	int id, mode;

	Trip_Data *trip_ptr;
	Time_Step time_step;

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (50000);

	if (data->Max_Records () == 0) {
		if (!data->Max_Records (file->Estimate_Records ())) goto mem_error;
	}

	//---- read the file ----

	while (file->Read ()) {
		Show_Progress ();

		//---- check the household id ----

		id = file->Household ();
		if (id < 1) continue;

		mode = file->Mode ();
		if (mode > 0 && mode < MAX_MODE && trip_mode [mode] == false) continue;

		if (hhlist_flag && (hhold_list.Get (&id) == NULL)) continue;

		trip_ptr = data->New_Record (true);
		if (trip_ptr == NULL) goto mem_error;

		trip_ptr->Household (id);
		trip_ptr->Person (file->Person ());

		if (traveler_flag && !travelers.In_Range (trip_ptr->Traveler ())) continue;

		trip_ptr->Trip (file->Trip ());

		trip_ptr->Origin (file->Origin ());
		trip_ptr->Destination (file->Destination ());
		trip_ptr->Vehicle (file->Vehicle ());
		trip_ptr->Mode (mode);
		trip_ptr->Constraint (file->Constraint ());

		trip_ptr->Start_Time (time_step.Step (file->Start ()));
		trip_ptr->End_Time (time_step.Step (file->Arrive ()));

		if (!data->Add ()) {
			Error ("Adding Record to the Trip Data Array");
		}
	}
	End_Progress ();

	file->Close ();
	data->Optimize ();
	return;

mem_error:
	Error ("Insufficient Memory for Trip Data");
}

