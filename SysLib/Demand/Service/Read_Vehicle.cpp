//*********************************************************
//	Read_Vehicle.cpp - Read the Vehicle File
//*********************************************************

#include "Demand_Service.hpp"

#include "Vehicle_File.hpp"

//---------------------------------------------------------
//	Read_Vehicle
//---------------------------------------------------------

void Demand_Service::Read_Vehicle (void)
{
	vehicle_data.Sorted (!sort_flag);

	Db_File *file = Demand_Db_File (VEHICLE);

	//---- store the vehicle data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (50000);

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (PARKING) && Network_Option_Flag (PARKING)) {
			Error (file_error, "Parking", file->File_ID ());
		}
	}
	//file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Vehicle_Processing (file)) {
			if (!vehicle_data.Add ()) {
				Error ("Adding Record to the Vehicle Data Array");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = vehicle_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Vehicle Data Records = %d", num);
	}
	vehicle_data.Optimize (compress_flag);

	if (num > 0) Demand_Data_True (VEHICLE);
}

//---------------------------------------------------------
//	Vehicle_Processing
//---------------------------------------------------------

bool Demand_Service::Vehicle_Processing (Db_File *fh)
{
	bool parking_flag;
	int id, lvalue, index;

	Vehicle_Data *vehicle_ptr;

	Vehicle_File *file = (Vehicle_File *) fh;

	parking_flag = Network_Data_Flag (PARKING);

	//---- reserve memory ----

	if (vehicle_data.Max_Records () == 0) {
		if (!vehicle_data.Max_Records (file->Estimate_Records ())) goto mem_error;
	}

	//---- check the vehicle id ----

	id = file->Vehicle ();
	if (id <= 0) return (false);

	vehicle_ptr = vehicle_data.New_Record (true);
	if (vehicle_ptr == NULL) goto mem_error;

	vehicle_ptr->Vehicle (id);

	vehicle_ptr->Household (file->Household ());

	lvalue = file->Location ();

	if (parking_flag) {
		index = parking_data.Get_Index (lvalue);

		if (index == 0) {
			Warning ("Vehicle %d Location %d was Not Found in the Parking File", id, lvalue);
			return (false);
		}
		if (Renumber ()) {
			lvalue = index;
		}
	}
	vehicle_ptr->Location (lvalue);

	vehicle_ptr->Type (file->Type ());
	vehicle_ptr->Sub_Type (file->Sub_Type ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Vehicle Data");
	return (false);
}
