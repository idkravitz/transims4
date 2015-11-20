//*********************************************************
//	Read_Household.cpp - Read the Household File
//*********************************************************

#include "Demand_Service.hpp"

#include "Household_File.hpp"

//---------------------------------------------------------
//	Read_Household
//---------------------------------------------------------

void Demand_Service::Read_Household (void)
{
	Db_File *file = Demand_Db_File (HOUSEHOLD);

	//---- store the household data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (50000);

	if (Check_Data () || Renumber ()) {
		if (!Network_File_Flag (ACTIVITY_LOCATION) && Network_Option_Flag (ACTIVITY_LOCATION)) {
			Error (file_error, "Activity Location", file->File_ID ());
		}
	}
	//file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();
		if (Household_Processing (file)) {
			if (!household_data.Add ()) {
				Error ("Adding Record to the Household Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = household_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Household Data Records = %d", num);
	}
	household_data.Optimize ();

	if (num > 0) Demand_Data_True (HOUSEHOLD);
}

//---------------------------------------------------------
//	Household_Processing
//---------------------------------------------------------

bool Demand_Service::Household_Processing (Db_File *fh)
{
	bool location_flag;
	int id, loc, index;

	Household_Data *household_ptr;

	Household_File *file = (Household_File *) fh;

	location_flag = Network_Data_Flag (ACTIVITY_LOCATION);

	//---- reserve memory ----

	if (household_data.Max_Records () == 0) {
		if (!household_data.Max_Records (file->Estimate_Records ())) goto mem_error;
	}

	//---- check the household id ----

	id = file->Household ();
	if (id <= 0) return (false);

	household_ptr = household_data.New_Record (true);
	if (household_ptr == NULL) goto mem_error;

	household_ptr->Household (id);

	loc = file->Location ();

	if (location_flag && loc != -1) {
		index = location_data.Get_Index (loc);

		if (index == 0) {
			Error ("Household %d Location %d was Not Found in the Activity Location File", id, loc);
		}
		if (Renumber ()) {
			loc = index;
		}
	}
	household_ptr->Location (loc);

	household_ptr->Persons (file->Persons ());
	household_ptr->Workers (file->Workers ());
	household_ptr->Vehicles (file->Vehicles ());
	household_ptr->Type (file->Type ());
	return (true);

mem_error:
	Error ("Insufficient Memory for Household Data");
	return (false);
}
