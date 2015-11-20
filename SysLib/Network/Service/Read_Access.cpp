//*********************************************************
//	Read_Access.cpp - read the process link file
//*********************************************************

#include "Network_Service.hpp"

#include "Access_File.hpp"

//---------------------------------------------------------
//	Read_Access
//---------------------------------------------------------

void Network_Service::Read_Access (void)
{
	Db_File *file = Network_Db_File (PROCESS_LINK);

	//---- store the access data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (PARKING) && Network_Option_Flag (PARKING)) {
			Error (file_error, "Parking", file->File_ID ());
		}
		if (!Network_File_Flag (ACTIVITY_LOCATION) && Network_Option_Flag (ACTIVITY_LOCATION)) {
			Error (file_error, "Activity Location", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Access_Processing (file)) {
			if (!access_data.Add ()) {
				Error ("Adding Record to the Access Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = access_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Process Link Data Records = %d", num);
	}
	access_data.Optimize ();

	if (num > 0) Network_Data_True (PROCESS_LINK);
}

//---------------------------------------------------------
//	Access_Processing
//---------------------------------------------------------

bool Network_Service::Access_Processing (Db_File *fh)
{
	bool parking_flag, location_flag, stop_flag, check_flag;
	int lvalue, index;

	Access_Data *access_ptr = NULL;

	Access_File *file = (Access_File *) fh;

	parking_flag = Network_Data_Flag (PARKING);
	location_flag = Network_Data_Flag (ACTIVITY_LOCATION);
	stop_flag = Network_Data_Flag (TRANSIT_STOP);
	check_flag = (Check_Data () || Renumber ());

	//---- reserve memory ----

	if (access_data.Max_Records () == 0) {
		if (!access_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	access_ptr = access_data.New_Record ();
	if (access_ptr == NULL) goto mem_error;

	access_ptr->ID (file->ID ());

	if (access_ptr->ID () == 0) return (false);

	//---- set the end type ----
	
	access_ptr->From_Type (End_Code (file->From_Type ()));

	if (check_flag && !stop_flag && access_ptr->From_Type () == STOP_ID) return (false);

	//---- check/convert the from id ----

	lvalue = file->From_ID ();

	if (check_flag) {
		if (access_ptr->From_Type () == LOCATION_ID) {
			index = location_data.Get_Index (lvalue);
		} else if (access_ptr->From_Type () == PARKING_ID) {
			index = parking_data.Get_Index (lvalue);
		} else if (access_ptr->From_Type () == STOP_ID) {
			index = stop_data.Get_Index (lvalue);
		} else {
			return (false);
		}
		if (index == 0) {
			return (Check_Message ("Process Link From_ID %d was Not Found in the %s file", lvalue, file->From_Type ()));
		}
		access_ptr->From_ID (Renumber () ? index : lvalue);
	} else {
		access_ptr->From_ID (lvalue);
	}

	//---- set the end type ----

	access_ptr->To_Type (End_Code (file->To_Type ()));
	
	if (check_flag && !stop_flag && access_ptr->To_Type () == STOP_ID) return (false);

	//---- check/convert the to id ----

	lvalue = file->To_ID ();

	if (check_flag) {
		if (access_ptr->To_Type () == LOCATION_ID) {
			index = location_data.Get_Index (lvalue);
		} else if (access_ptr->To_Type () == PARKING_ID) {
			index = parking_data.Get_Index (lvalue);
		} else if (access_ptr->To_Type () == STOP_ID) {
			index = stop_data.Get_Index (lvalue);
		} else {
			return (false);
		}
		if (index == 0) {
			return (Check_Message ("Process Link To_ID %d was Not Found in the %s file", lvalue, file->To_Type ()));
		}
		access_ptr->To_ID (Renumber () ? index : lvalue);
	} else {
		access_ptr->To_ID (lvalue);
	}

	//----- optional fields ----

	access_ptr->Time (Round (file->Time ()));
	access_ptr->Cost ((int) (file->Cost () + 0.5));

	return (true);

mem_error:
	Error ("Insufficient Memory for Access Data Records");
	return (false);
}
