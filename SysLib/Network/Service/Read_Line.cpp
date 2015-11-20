//*********************************************************
//	Read_Line.cpp - read the transit route file
//*********************************************************

#include "Network_Service.hpp"

#include "Line_File.hpp"

//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void Network_Service::Read_Line (void)
{
	Line_File *file = (Line_File *) Network_Db_File (TRANSIT_ROUTE);

	//---- store the transit route data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) {
		if (!Network_Data_Flag (TRANSIT_STOP) && 
			(Network_File_Flag (TRANSIT_STOP) || Network_Option_Flag (TRANSIT_STOP))) {
			Error (file_error, "Transit Stop", file->File_ID ());
		}
	}
	First_Record (true);
	int i, num;

	while (file->Read (false)) {
		Show_Progress ();

		if (Line_Processing (file)) {
			if (!line_data.Add ()) goto add_error;

		}
		First_Record (false);

		num = file->Stops ();

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error ("Number of Nested Records for Route %d", file->Route ());
			}
			Show_Progress ();

			if (Line_Processing (file)) {
				if (!line_data.Add ()) goto add_error;
			}			
		}	
	}
	End_Progress ();
	file->Close ();

	line_data.Route_Records (Progress_Count ());

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	num = line_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Transit Route Data Records = %d", num);
	}
	line_data.Optimize ();

	if (num > 0) Network_Data_True (TRANSIT_ROUTE);
	return;

add_error:
	Error ("Adding Record to the Transit Route Data List");
}

//---------------------------------------------------------
//	Line_Processing
//---------------------------------------------------------

bool Network_Service::Line_Processing (Db_File *fh)
{
	static int id, nstops, stop_num; 
	int zone;

	bool stop_flag;
	
	Line_Data *line_ptr = NULL;

	Line_File *file = (Line_File *) fh;

	stop_flag = Network_Data_Flag (TRANSIT_STOP);

	//---- process a header line ----

	if (!file->Nest ()) {	
		id = file->Route ();

		if (id == 0) return (false);

		if (id < 1) {
			return (Check_Message ("Transit Route ID %d is Out of Range", id));
		}
		line_ptr = line_data.New_Record (true);
		if (line_ptr == NULL) goto mem_error;

		line_ptr->Route (id);

		stop_num = 1;
		nstops = file->Stops ();

		if (nstops < 2) {
			return (Check_Message ("Number of Stops %d on Route %d is Out of Range", nstops, id));
		}
		if (!line_ptr->Stops (nstops)) {
			Error ("Insufficient Memory for %d Stops on Router %d", nstops, id);
		}
		line_ptr->Mode (Transit_Code (file->Mode ()));
		line_ptr->Null_Name ();
		line_ptr->Null_Notes ();

		if (Notes_Name_Flag ()) {
			char *str_ptr = file->Name ();

			if (str_ptr != NULL && *str_ptr != '\0') {
				line_ptr->Name (str_ptr);
			}
			str_ptr = file->Notes ();

			if (str_ptr != NULL && *str_ptr != '\0') {
				line_ptr->Notes (str_ptr);
			}
		}
	} else {
		line_ptr = line_data.New_Record (false);
		if (line_ptr == NULL) goto mem_error;

		//---- process a stop record ----

		int stop = file->Stop ();

		if (stop_flag) {
			int index = stop_data.Get_Index (stop);

			if (index == 0) {
				return (Check_Message ("Transit Stop %d on Route %d was Not Found in the Stop File", stop, id));
			}
			if (Renumber ()) stop = index;
		}
		line_ptr->Stop (stop_num, stop);

		zone = file->Zone ();

		if (Num_Fare_Zones () > 0 && (zone < 1 || zone > Num_Fare_Zones ())) {
			if (zone != 0 || !Zero_Fare_Zone ()) {
				Warning ("Transit Zone %d on Route %d is Out of Range (1..%d)", zone, id, Num_Fare_Zones ());
				if (zone == 0) Zero_Fare_Zone (true);
			}
		}
		line_ptr->Zone (stop_num, zone);
		line_ptr->Time_Flag (stop_num, file->Time_Flag ());

		if (stop_num == nstops) {
			return (true);
		} else {
			stop_num++;
			return (false);
		}
	}
	return (false);

mem_error:
	Error ("Insuffienct Memory for Transit Line Data");
	return (false);
}
