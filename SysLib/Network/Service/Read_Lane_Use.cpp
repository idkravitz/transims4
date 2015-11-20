//*********************************************************
//	Read_Lane_Use.cpp - read the lane use file
//*********************************************************

#include "Network_Service.hpp"

#include "Lane_Use_File.hpp"

//---------------------------------------------------------
//	Read_Lane_Use
//---------------------------------------------------------

void Network_Service::Read_Lane_Use (void)
{
	Lane_Use_File *file = (Lane_Use_File *) Network_Db_File (LANE_USE);

	//---- store the lane use data ----

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

		if (Lane_Use_Processing (file)) {
			if (!lane_use_data.Add ()) {
				Error ("Adding Record to the Lane Use Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = lane_use_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Lane Use Data Records = %d", num);
	}
	lane_use_data.Optimize ();

	if (num > 0) Network_Data_True (LANE_USE);

	//---- create the link use restrictions ----

	if (link_data.Num_Records () > 0 && lane_use_data.Num_Records () > 0) {
		Time_of_Day_Restrictions ();
	}
}

//---------------------------------------------------------
//	Lane_Use_Processing
//---------------------------------------------------------

bool Network_Service::Lane_Use_Processing (Db_File *fh)
{
	static bool lane_flag;

	int link, dir, index, lane;
	bool node_flag, link_flag;
	char *str_ptr;

	Time_Step time_step;
	Lane_Use_Data *lane_use_ptr;
	Link_Data *link_ptr = NULL;

	Lane_Use_File *file = (Lane_Use_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (lane_use_data.Max_Records () == 0) {
	
		//---- set flags ----

		lane_flag = (file->Field ("LANES") != NULL);

		if (!lane_use_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
		
	lane_use_ptr = lane_use_data.New_Record ();
	if (lane_use_ptr == NULL) goto mem_error;

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
			return (Check_Message ("Lane Use Link %d was Not Found in the Link File", link));
		}
		if (file->LinkDir_Type () == LINK_NODE) {
			if (Renumber ()) {
				index = node_data.Get_Index (dir);

				if (index == 0) {
					return (Check_Message ("Node %d on Lane Use Link %d was Not Found in the Node File", dir, link));
				}
				dir = index;
			}
			if (dir == link_ptr->Anode ()) {
				dir = 1;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 0;
			} else {
				if (Renumber ()) {
					dir = node_data [dir]->ID ();
				}
				return (Check_Message ("Node %d and Lane Use Link %d are Incompatible", dir, link));
			}
		}
		if (Renumber ()) link = link_data.Record_Index ();
	}
	lane_use_ptr->Link (link);
	lane_use_ptr->Dir (dir);

	//---- lane number ----

	lane = file->Lane ();

	if (link_flag) {	
		Dir_Data *dir_ptr;

		if (lane_use_ptr->Dir ()) {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		}
		if (dir_ptr == NULL) {
			return (Check_Message ("Link Direction on Lane Use Link %d", link_ptr->Link ()));
		}
		index = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ();

		if (lane < 0 || lane > index) {
			//return (Check_Message ("Lane Use Lane %d is Not on Link %d", lane, link_ptr->Link ()));
			Warning ("Lane Use Lane %d is Not on Link %d", lane, link_ptr->Link ());
			return (false);
		}
		lane_use_ptr->Lane (lane);
	} else {
		lane_use_ptr->Lane (lane);
	}

	//---- use permission ----

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "ANY";
	}
	lane_use_ptr->Use (Use_Code (str_ptr));

	//----- optional fields ----

	lane_use_ptr->Type (Restrict_Code (file->Type ()));

	str_ptr = file->Start ();

	if (*str_ptr == '\0') {
		lane_use_ptr->Start (0);
	} else {
		lane_use_ptr->Start (Round (time_step.Step (str_ptr)));
	}
	str_ptr = file->End ();

	if (*str_ptr == '\0') {
		lane_use_ptr->End (Round (MIDNIGHT));
	} else {
		lane_use_ptr->End (Round (time_step.Step (str_ptr)));
	}
	lane_use_ptr->Offset (Round (file->Offset ()));
	lane_use_ptr->Length (Round (file->Length ()));

	if (link_flag) {
		if (lane_use_ptr->Offset () < 0 || lane_use_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Lane Use %d Offset %.1lf is Out of Range (0..%.1lf)",
			link_ptr->ID (), UnRound (lane_use_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (lane_use_ptr->Offset () < 0) {
				lane_use_ptr->Offset (0);
			} else {
				lane_use_ptr->Offset (link_ptr->Length ());
			}
		}
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Lane Use Data");
	return (false);
}
