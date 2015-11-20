//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "Demand_Service.hpp"

#include "Trip_File.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

void Demand_Service::Read_Trip (void)
{
	Db_File *file = Demand_Db_File (TRIP);

	//---- store the trip data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (50000);

	if (Check_Data () || Renumber ()) {
		if (!Network_File_Flag (ACTIVITY_LOCATION) && Network_Option_Flag (ACTIVITY_LOCATION)) {
			Error (file_error, "Activity Location", file->File_ID ());
		}
		if (!Demand_File_Flag (VEHICLE) && Demand_Option_Flag (VEHICLE)) {
			Error (file_error, "Vehicle", file->File_ID ());
		}
	}	
	//file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Trip_Processing (file)) {
			if (!trip_data.Add ()) {
				Error ("Adding Record to the Trip Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = trip_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Trip Data Records = %d", num);
	}
	trip_data.Optimize ();

	if (num > 0) Demand_Data_True (TRIP);
}

//---------------------------------------------------------
//	Trip_Processing
//---------------------------------------------------------

bool Demand_Service::Trip_Processing (Db_File *fh)
{
	static bool location_flag, vehicle_flag;
	int id, lvalue, index;

	Trip_Data *trip_ptr;
	Time_Step time_step;

	Trip_File *file = (Trip_File *) fh;

	//---- reserve memory ----

	if (First_Record ()) {
		location_flag = (location_array != NULL && location_data.Num_Records () > 0);
		vehicle_flag = (vehicle_array != NULL && vehicle_data.Num_Records () > 0);

		if (trip_data.Max_Records () == 0) {
			if (!trip_data.Max_Records (file->Estimate_Records ())) goto mem_error;
		}
	}

	//---- check the household id ----

	id = file->Household ();
	if (id < 1) return (false);

	trip_ptr = trip_data.New_Record (true);
	if (trip_ptr == NULL) goto mem_error;

	trip_ptr->Household (id);

	trip_ptr->Person (file->Person ());
	trip_ptr->Trip (file->Trip ());

	lvalue = file->Origin ();

	if (location_flag) {
		index = location_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Household %d Trip Origin %d was Not Found", id, lvalue);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	trip_ptr->Origin (lvalue);

	lvalue = file->Destination ();

	if (location_flag) {
		index = location_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Household %d Trip Destination %d was Not Found", id, lvalue);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	trip_ptr->Destination (lvalue);

	lvalue = file->Vehicle ();

	if (vehicle_flag) {
		index = vehicle_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Household %d Vehicle %d was Not Found", id, lvalue);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	trip_ptr->Vehicle (lvalue);
	trip_ptr->Mode (file->Mode ());
	trip_ptr->Constraint (file->Constraint ());

	trip_ptr->Start_Time (time_step.Step (file->Start ()));
	trip_ptr->End_Time (time_step.Step (file->Arrive ()));

	return (true);

mem_error:
	Error ("Insufficient Memory for Trip Data");
	return (false);
}
