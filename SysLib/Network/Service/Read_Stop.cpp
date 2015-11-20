//*********************************************************
//	Read_Stop.cpp - read the transit stop file
//*********************************************************

#include "Network_Service.hpp"

#include "Stop_File.hpp"

//---------------------------------------------------------
//	Read_Stop
//---------------------------------------------------------

void Network_Service::Read_Stop (void)
{
	Stop_File *file = (Stop_File *) Network_Db_File (TRANSIT_STOP);

	//---- store the transit stop data ----

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
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Stop_Processing (file)) {
			if (!stop_data.Add ()) {
				Error ("Adding Record to the Transit Stop Data List");				
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = stop_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Transit Stop Data Records = %d", num);
	}
	stop_data.Optimize ();

	if (num > 0) Network_Data_True (TRANSIT_STOP);
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool Network_Service::Stop_Processing (Db_File *fh)
{
	bool node_flag, link_flag;
	int id, link, dir, index;
	char *str_ptr;

	Link_Data *link_ptr = NULL;
	Stop_Data *stop_ptr = NULL;

	Stop_File *file = (Stop_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (stop_data.Max_Records () == 0) {
		if (!stop_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	stop_ptr = (Stop_Data *) stop_data.New_Record ();
	if (stop_ptr == NULL) goto mem_error;

	id = file->ID ();
	if (id == 0) return (false);

	stop_ptr->Stop (id);

	//---- check/convert the link number ----

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
			return (Check_Message ("Link %d on Stop %d was Not Found in the Link File", link, id));
		}
		if (file->LinkDir_Type () == LINK_NODE) {
			if (Renumber ()) {
				index = node_data.Get_Index (dir);

				if (index == 0) {
					return (Check_Message ("Node %d on Stop %d was Not Found in the Node File", dir, id));
				}
				dir = index;
			}
			if (dir == link_ptr->Anode ()) {
				dir = 1;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 0;
			} else {
				return (Check_Message ("Node %d Link %d on Stop %d are Incompatible", node_data [dir]->ID (), link, id));
			}
		}
		if (Renumber ()) link = link_data.Record_Index ();
	}
	stop_ptr->Link (link);
	stop_ptr->Dir (dir);

	//----- round the link offset ----

	stop_ptr->Offset (Round (file->Offset ()));
	
	if (link_flag) {
		if (stop_ptr->Offset () < 0 || stop_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Transit Stop %d Offset %.1lf is Out of Range (0..%.1lf)",
				stop_ptr->ID (), UnRound (stop_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (stop_ptr->Offset () < 0) {
				stop_ptr->Offset (0);
			} else {
				stop_ptr->Offset (link_ptr->Length ());
			}
		}
		if (stop_ptr->Dir () == 0) {		//---- offset from A node ----
			stop_ptr->Offset (link_ptr->Length () - stop_ptr->Offset ());
		}
	}

	//----- optional fields ----

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "BUS/RAIL";
	}
	stop_ptr->Use (Use_Code (str_ptr));

	if (file->Type_Flag ()) {
		stop_ptr->Type (Stop_Code (file->Type ()));
	} else {
		stop_ptr->Type (STOP);
	}
	stop_ptr->Space (file->Space ());
	stop_ptr->Traveler (0);

	stop_ptr->Null_Name ();
	stop_ptr->Null_Notes ();

	if (Notes_Name_Flag ()) {
		char *str_ptr = file->Name ();

		if (str_ptr != NULL && *str_ptr != '\0') {
			stop_ptr->Name (str_ptr);
		}
		str_ptr = file->Notes ();

		if (str_ptr != NULL && *str_ptr != '\0') {
			stop_ptr->Notes (str_ptr);
		}
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Transit Stop Data");
	return (false);
}
