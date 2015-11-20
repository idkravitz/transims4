//*********************************************************
//	Read_Route_Header.cpp - read the route header file
//*********************************************************

#include "Network_Service.hpp"

#include "Route_Header_File.hpp"

//---------------------------------------------------------
//	Read_Route_Header
//---------------------------------------------------------

void Network_Service::Read_Route_Header (void)
{
	Db_File *file = Network_Db_File (ROUTE_HEADER);

	//---- store the node data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Route_Header_Processing (file)) {
			if (!route_data.Add ()) {
				Error ("Adding Route Header Data");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = route_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Route Data Records = %d", num);
	}
	route_data.Optimize ();

	if (num > 0) Network_Data_True (ROUTE_HEADER);
}

//---------------------------------------------------------
//	Route_Header_Processing
//---------------------------------------------------------

bool Network_Service::Route_Header_Processing (Db_File *fh)
{
	int i, offset;

	Route_Data *route_ptr;

	Route_Header_File *file = (Route_Header_File *) fh;

	//---- reserve memory ----

	if (route_data.Max_Records () == 0) {
		if (!route_data.Max_Records (file->Num_Records ())) goto mem_error;

		route_data.Num_Periods (file->Periods ());
		route_data.Num_Nodes (0);
	}

	//---- read and save the data ----

	route_ptr = route_data.New_Record (true);
	if (route_ptr == NULL) goto mem_error;

	route_ptr->Route (file->Route ());
	if (route_ptr->Route () == 0) return (false);

	route_ptr->Mode (Transit_Code (file->Mode ()));
	route_ptr->Min_Time (file->Min_Time () * 60);

	if (!route_ptr->Periods (file->Periods ())) goto mem_error;

	for (i=1; i <= file->Periods (); i++) {
		offset = file->Offset (i);
		if (offset > 0) {
			offset *= 60;
		}
		route_ptr->Headway (i, file->Headway (i) * 60);
		route_ptr->Offset (i, offset);
		route_ptr->TTime (i, file->TTime (i) * 60);
	}
	route_ptr->Null_Name ();
	route_ptr->Null_Notes ();

	if (Notes_Name_Flag ()) {
		char *str_ptr = file->Name ();

		if (str_ptr != NULL && *str_ptr != '\0') {
			route_ptr->Name (str_ptr);
		}
		str_ptr = file->Notes ();

		if (str_ptr != NULL && *str_ptr != '\0') {
			route_ptr->Notes (str_ptr);
		}
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Route Data");
	return (false);
}
