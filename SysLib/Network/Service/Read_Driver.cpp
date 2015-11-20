//*********************************************************
//	Read_Driver.cpp - read the transit driver file
//*********************************************************

#include "Network_Service.hpp"

#include "Driver_File.hpp"

//---------------------------------------------------------
//	Read_Driver
//---------------------------------------------------------

void Network_Service::Read_Driver (void)
{
	Driver_File *file = (Driver_File *) Network_Db_File (TRANSIT_DRIVER);

	//---- store the transit driver data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) {
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE) &&
			file->LinkDir_Type () == LINK_NODE) { 
			Error (file_error, "Node", file->File_ID ());
		}
		if (!Network_File_Flag (LINK) && Network_Option_Flag (LINK)) {
			Error (file_error, "Link", file->File_ID ());
		}
		if (!Network_File_Flag (TRANSIT_ROUTE) && Network_Option_Flag (TRANSIT_ROUTE)) {
			Error (file_error, "Transit Route", file->File_ID ());
		}
	}
	First_Record (true);
	int i, num;

	while (file->Read (false)) {
		Show_Progress ();

		if (Driver_Processing (file)) {
			if (!driver_data.Add ()) goto add_error;
		}
		First_Record (false);

		num = file->Links ();

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error ("Number of Nested Records for Route %d", file->Route ());
			}
			Show_Progress ();

			if (Driver_Processing (file)) {
				if (!driver_data.Add ()) goto add_error;
			}			
		}
	}
	End_Progress ();
	file->Close ();

	driver_data.Driver_Records (Progress_Count ());

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	num = driver_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Transit Driver Data Records = %d", num);
	}
	if (num > 0) Network_Data_True (TRANSIT_DRIVER);

	//---- check for driver plans for each route ----
		
	if (line_array != NULL && line_data.Num_Records () > 0) {
		Line_Data *line_ptr;
		Driver_Data *driver_ptr;
		
		bool stop_flag = (stop_array != NULL && stop_data.Num_Records () > 0);

		for (line_ptr = line_data.First (); line_ptr != NULL; line_ptr = line_data.Next ()) {

			driver_ptr = driver_data.Get (line_ptr->Route ());

			if (driver_ptr == NULL) {
				Check_Message ("Driver Plan was Not Found for Route %d", line_ptr->Route ());
			}
			line_ptr->Driver (driver_data.Record_Index ());

			//---- check the stop on the route ----

			if (stop_flag) {
				int i, j, stop;
				Stop_Data *stop_ptr;

				for (i=j=1; i <= line_ptr->Stops (); i++) {
					stop = line_ptr->Stop (i);

					if (Renumber ()) {
						stop_ptr = stop_data [stop];
					} else {
						stop_ptr = stop_data.Get (stop);
					}
					for (; j <= driver_ptr->Links (); j++) {
						if (stop_ptr->Link_Dir () == driver_ptr->Link_Dir (j)) break;
					}
					if (j > driver_ptr->Links ()) {
						Check_Message ("Route %d Stops and Driver Links are Incompatible", line_ptr->Route ());
					}
				}
			}
		}
	}

	driver_data.Optimize ();
	return;

add_error:
	Error ("Adding Record to the Transit Driver Data List");
}

//---------------------------------------------------------
//	Driver_Processing
//---------------------------------------------------------

bool Network_Service::Driver_Processing (Db_File *fh)
{
	static int id, nlinks, link_num; 

	bool node_flag, link_flag, line_flag;
	
	Driver_Data *driver_ptr = NULL;
	Link_Data *link_ptr = NULL;
	Line_Data *line_ptr = NULL;

	Driver_File *file = (Driver_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);
	line_flag = Network_Data_Flag (TRANSIT_ROUTE);

	//---- process a header line ----

	if (!file->Nest ()) {	
		id = file->Route ();

		if (id == 0) return (false);

		if (id < 1) {
			return (Check_Message ("Transit Route ID %d is Out of Range", id));
		}
		driver_ptr = driver_data.New_Record (true);
		if (driver_ptr == NULL) goto mem_error;

		if (line_flag) {
			line_ptr = line_data.Get (id);

			if (line_ptr == NULL) {
				return (Check_Message ("Transit Route %d was Not Found in the Route File", id));
			}
		}
		driver_ptr->Route (id);

		link_num = 1;
		nlinks = file->Links ();

		if (nlinks < 1) {
			return (Check_Message ("Number of Links %d on Route %d is Out of Range", nlinks, id));
		}
		if (!driver_ptr->Links (nlinks)) {
			Error ("Insufficient Memory for %d Links on Router %d", nlinks, id);
		}
		driver_ptr->Type (file->Veh_Type ());
		driver_ptr->Sub_Type (file->Sub_Type ());

		if (line_flag) {
			Transit_Type mode = (Transit_Type) (line_ptr->Mode ());

			if (Veh_Type_Map (driver_ptr->Type ()) != Transit_Use_Map (mode)) {
				return (Check_Message ("Route %d Vehicle Type %d is incompatible with %s", 
					id, driver_ptr->Type (), Transit_Code (mode)));
			}
		}

		//---- notes string ----

		driver_ptr->Null_Notes ();

		if (Notes_Name_Flag ()) {
			char *str_ptr = file->Notes ();

			if (str_ptr != NULL && *str_ptr != '\0') {
				driver_ptr->Notes (str_ptr);
			}
		}
	} else {

		driver_ptr = driver_data.New_Record (false);
		if (driver_ptr == NULL) goto mem_error;

		//---- process a link record ----

		int link, dir, index, node;

		link = file->Link ();
		dir = file->Dir ();

		if (file->LinkDir_Type () == LINK_SIGN) {
			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}
		if (link_flag) {
			link_ptr = link_data.Get (link);

			if (link_ptr == NULL) {
				return (Check_Message ("Link %d on Route %d was Not Found in the Link File", link, id));
			}
			if (file->LinkDir_Type () == LINK_NODE) {
				node = dir;

				if (Renumber ()) {
					index = node_data.Get_Index (node);

					if (index == 0) {
						return (Check_Message ("Node %d on Route %d was Not Found in the Node File", node, id));
					}
					dir = index;
				}
				if (dir == link_ptr->Anode ()) {
					dir = 1;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 0;
				} else {
					return (Check_Message ("Node %d Link %d on Route %d are Incompatible", node, link, id));
				}
			}
			if (Renumber ()) link = link_data.Record_Index ();
		}
		driver_ptr->Link (link_num, link);
		driver_ptr->Dir (link_num, dir);

		if (link_num == nlinks) {
			return (true);
		} else {
			link_num++;
			return (false);
		}
	}
	return (false);

mem_error:
	Error ("Insuffienct Memory for Transit Driver Data");
	return (false);
}
