//*********************************************************
//	Read_Boundary.cpp - Read the Boundary File
//*********************************************************

#include "Demand_Service.hpp"

#include "Boundary_File.hpp"

//---------------------------------------------------------
//	Read_Boundary
//---------------------------------------------------------

void Demand_Service::Read_Boundary (void)
{
	Db_File *file = Demand_Db_File (BOUNDARY_SPEED);

	//---- store the boundary data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (PARKING) && Network_Option_Flag (PARKING)) {
			Error (file_error, "Parking", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Boundary_Processing (file)) {
			if (!boundary_data.Add ()) {
				Error ("Adding Record to the Boundary Speed Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = boundary_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Boundary Speed Records = %d", num);
	}
	boundary_data.Optimize ();

	if (num > 0) Demand_Data_True (BOUNDARY_SPEED);
}

//---------------------------------------------------------
//	Boundary_Processing
//---------------------------------------------------------

bool Demand_Service::Boundary_Processing (Db_File *fh)
{
	static bool parking_flag;

	int lot, index, start, end, period, speed, increment;
	bool stat;

	Boundary_Data *boundary_ptr;
	Time_Step time_step;

	Boundary_File *file = (Boundary_File *) fh;

	//---- reserve memory ----

	if (First_Record ()) {
		parking_flag = (parking_array != NULL && parking_data.Num_Records () > 0);

		start = time_step.Step (file->Start_Time ());
		end = time_step.Step (file->End_Time ());
		increment = end - start;
		if (increment <= 0) increment = 900;

		boundary_data.Period_Size (increment);

		period = boundary_data.Periods ();
		if (period == 0) {
			Error ("Boundary Periods are Zero");
		}
		if (boundary_data.Max_Records () == 0) {
			if (!boundary_data.Max_Records (file->Num_Records () / period)) goto mem_error;
		}
	}

	//---- check the household id ----

	lot = file->Parking ();
	if (lot < 1) return (false);

	if (parking_flag) {
		index = parking_data.Get_Index (lot);

		if (index == 0) {
			Error ("Boundary Parking Lot %d was Not Found", lot);
		}
		if (Renumber ()) {
			lot = index;
		}
	}
	boundary_ptr = boundary_data.Get (lot);

	if (boundary_ptr == NULL) {
		boundary_ptr = boundary_data.New_Record (true);
		if (boundary_ptr == NULL) goto mem_error;
		if (!boundary_ptr->Periods (boundary_data.Periods ())) goto mem_error;
		boundary_ptr->Parking (lot);
		stat = true;
	} else {
		stat = false;
	}
	start = time_step.Step (file->Start_Time ());
	end = time_step.Step (file->End_Time ());
	speed = Round (file->Speed ());

	increment = boundary_data.Period_Size ();
	period = boundary_data.Period (start);

	for (; start < end; start += increment, period++) {
		boundary_ptr->Speed (period, speed);
	}
	return (stat);

mem_error:
	Error ("Insufficient Memory for Boundary Speed Data");
	return (false);
}
