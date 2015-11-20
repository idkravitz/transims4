//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "Demand_Service.hpp"

#include "Activity_File.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void Demand_Service::Read_Activity (void)
{
	Db_File *file = Demand_Db_File (ACTIVITY);

	//---- store the activity data ----

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

		if (Activity_Processing (file)) {
			if (!activity_data.Add ()) {
				Error ("Adding Record to the Activity Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = activity_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Activity Data Records = %d", num);
	}
	activity_data.Optimize ();

	if (num > 0) Demand_Data_True (ACTIVITY);
}

//---------------------------------------------------------
//	Activity_Processing
//---------------------------------------------------------

bool Demand_Service::Activity_Processing (Db_File *fh)
{
	bool location_flag, vehicle_flag;
	int id, lvalue, index;
	
	Time_Step time_step (1, "HOURS");

	Activity_Data *activity_ptr;

	Activity_File *file = (Activity_File *) fh;

	location_flag = Network_Data_Flag (ACTIVITY_LOCATION);
	vehicle_flag = Demand_Data_Flag (VEHICLE);

	//---- reserve memory ----

	if (activity_data.Max_Records () == 0) {
		if (!activity_data.Max_Records (file->Estimate_Records ())) goto mem_error;
	}

	//---- check the household id ----

	id = file->Household ();
	if (id < 1) return (false);

	activity_ptr = activity_data.New_Record (true);
	if (activity_ptr == NULL) goto mem_error;

	activity_ptr->Household (id);

	activity_ptr->Person (file->Person ());
	activity_ptr->Activity (file->Activity ());

	lvalue = file->Location ();

	if (location_flag) {
		index = location_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Household %d Activity Location %d was Not Found", id, lvalue);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	activity_ptr->Location (lvalue);

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
	activity_ptr->Vehicle (lvalue);

	activity_ptr->Mode (file->Mode ());
	activity_ptr->Purpose (file->Purpose ());
	activity_ptr->Passengers (file->Passengers ());
	activity_ptr->Constraint (file->Constraint ());
	activity_ptr->Priority (file->Priority ());

	lvalue = time_step.Step (file->Start_Min ());
	lvalue = (lvalue + time_step.Step (file->Start_Max ()) + 1) / 2;

	activity_ptr->Start_Time (lvalue);

	lvalue = time_step.Step (file->End_Min ());
	lvalue = (lvalue + time_step.Step (file->End_Max ()) + 1) / 2;
	
	activity_ptr->End_Time (lvalue);

	lvalue = time_step.Duration (file->Time_Min ());
	lvalue = (lvalue + time_step.Duration (file->Time_Max ()) + 1) / 2;
	
	activity_ptr->Duration (lvalue);
	return (true);

mem_error:
	Error ("Insufficient Memory for Activity Data");
	return (false);
}
