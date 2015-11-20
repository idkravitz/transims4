//*********************************************************
//	Read_Toll.cpp - read the toll file
//*********************************************************

#include "Network_Service.hpp"

#include "Toll_File.hpp"

//---------------------------------------------------------
//	Read_Toll
//---------------------------------------------------------

void Network_Service::Read_Toll (void)
{
	Toll_File *file = (Toll_File *) Network_Db_File (TOLL);

	//---- store the toll data ----

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

		if (Toll_Processing (file)) {
			if (!toll_data.Add ()) {
				Error ("Adding Record to the Toll Data List");				
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = toll_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Toll Data Records = %d", num);
	}
	toll_data.Optimize ();

	if (num > 0) Network_Data_True (TOLL);
}

//---------------------------------------------------------
//	Toll_Processing
//---------------------------------------------------------

bool Network_Service::Toll_Processing (Db_File *fh)
{
	bool node_flag, link_flag;
	int link, dir, index;
	char *str_ptr;

	Time_Step time_step;
	Link_Data *link_ptr = NULL;
	Toll_Data *toll_ptr = NULL;

	Toll_File *file = (Toll_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (toll_data.Max_Records () == 0) {
		if (!toll_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	toll_ptr = (Toll_Data *) toll_data.New_Record ();
	if (toll_ptr == NULL) goto mem_error;

	link = file->Link ();

	if (link == 0) return (false);

	//---- check/convert the link number ----

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
			return (Check_Message ("Toll Link %d was Not Found in the Link File", link));
		}
		if (file->LinkDir_Type () == LINK_NODE) {
			if (Renumber ()) {
				index = node_data.Get_Index (dir);

				if (index == 0) {
					return (Check_Message ("Node %d on Toll Link %d was Not Found in the Node File", dir, link));
				}
				dir = index;
			}
			if (dir == link_ptr->Anode ()) {
				dir = 1;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 0;
			} else {
				return (Check_Message ("Node %d and Toll Link %d are Incompatible", node_data [dir]->ID (), link));
			}
		}
		if (Renumber ()) link = link_data.Record_Index ();
	}
	toll_ptr->Link (link);
	toll_ptr->Dir (dir);

	toll_ptr->Toll (file->Toll ());

	//----- optional fields ----

	str_ptr = file->Start ();

	if (*str_ptr == '\0') {
		toll_ptr->Start (0);
	} else {
		toll_ptr->Start (Round (time_step.Step (str_ptr)));
	}
	str_ptr = file->End ();

	if (*str_ptr == '\0') {
		toll_ptr->End (Round (MIDNIGHT));
	} else {
		toll_ptr->End (Round (time_step.Step (str_ptr)));
	}

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "ANY";
	}
	toll_ptr->Use (Use_Code (str_ptr));

	toll_ptr->TOD_List (0);

	return (true);

mem_error:
	Error ("Insufficient Memory for Toll Data");
	return (false);
}
