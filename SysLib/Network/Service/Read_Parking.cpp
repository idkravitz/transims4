//*********************************************************
//	Read_Parking.cpp - read the parking file
//*********************************************************

#include "Network_Service.hpp"

#include "Parking_File.hpp"

//---------------------------------------------------------
//	Read_Parking
//---------------------------------------------------------

void Network_Service::Read_Parking (void)
{
	Parking_File *file = (Parking_File *) Network_Db_File (PARKING);

	//---- store the parking data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) {
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE) &&
			file->LinkDir_Type () == LINK_NODE) { 
			Error (file_error, "Node", file->File_ID ());
		}
		if (!Network_File_Flag (LINK) || Network_Option_Flag (LINK)) {
			Error (file_error, "Link", file->File_ID ());
		}
	}
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Parking_Processing (file)) {
			if (!parking_data.Add ()) {
				Error ("Adding Record to the Parking Data List");				
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = parking_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Parking Data Records = %d", num);
	}
	parking_data.Optimize ();

	if (num > 0) Network_Data_True (PARKING);
}

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool Network_Service::Parking_Processing (Db_File *fh)
{
	static bool dir_flag;

	bool node_flag, link_flag;
	char *str_ptr;
	int lvalue, index;

	Time_Step time_step;	
	Parking_Data *parking_ptr = NULL;
	Link_Data *link_ptr = NULL;

	Parking_File *file = (Parking_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (parking_data.Max_Records () == 0) {
		dir_flag = (file->Field ("DIR") != NULL);

		if (!parking_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
	
	parking_ptr = parking_data.New_Record ();
	if (parking_ptr == NULL) goto mem_error;

	parking_ptr->ID (file->ID ());

	if (parking_ptr->ID () == 0) return (false);

	//---- check/convert the link number ----
	
	lvalue = file->Link ();

	if (link_flag) {
		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Parking Link %d was Not Found in the Link file", lvalue));
		}
		link_ptr = link_data [index];

		parking_ptr->Link (Renumber () ? index : lvalue);
	} else {
		parking_ptr->Link (lvalue);
	}

	//---- determine the link direction ----

	lvalue = file->Node ();

	if (dir_flag) {
		parking_ptr->Dir (lvalue);
	} else if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Parking Node %d was Not Found in the Node file", lvalue));
		}
		if (link_flag) {
			if (!Renumber ()) {
				index = lvalue;
			}
			if (link_ptr->Anode () == index) {
				parking_ptr->Dir (1);
			} else if (link_ptr->Bnode () == index) {
				parking_ptr->Dir (0);
			} else {
				goto link_error;
			}
		} else {
			parking_ptr->Dir (0);
		}
	} else if (link_flag) {
		if (link_ptr->Anode () == lvalue) {
			parking_ptr->Dir (1);
		} else if (link_ptr->Bnode () == lvalue) {
			parking_ptr->Dir (0);
		} else {
			goto link_error;
		}
	} else {
		parking_ptr->Dir (0);
	}

	//----- round the link offset ----

	parking_ptr->Offset (Round (file->Offset ()));	

	if (link_flag) {
		if (parking_ptr->Offset () < 0 || parking_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Parking Lot %d Offset %.1lf is Out of Range (0..%.1lf)",
				parking_ptr->ID (), UnRound (parking_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (parking_ptr->Offset () < 0) {
				parking_ptr->Offset (0);
			} else {
				parking_ptr->Offset (link_ptr->Length ());
			}
		}
		if (node_flag || dir_flag) {
			if (parking_ptr->Dir () == 0) {		//---- offset from A node ----
				parking_ptr->Offset (link_ptr->Length () - parking_ptr->Offset ());
			}
		}
	}

	//---- set the parking type ----
	
	parking_ptr->Type (Parking_Code (file->Type ()));

	//---- use permission ----

	str_ptr = file->Use ();

	if (*str_ptr == '\0') {
		str_ptr = "ANY";
	}
	parking_ptr->Use (Use_Code (str_ptr));

	//----- optional fields ----

	parking_ptr->Space (file->Space ());

	str_ptr = file->Start ();

	if (*str_ptr == '\0') {
		parking_ptr->Start (0);
	} else {
		parking_ptr->Start (Round (time_step.Step (str_ptr)));
	}
	str_ptr = file->End ();

	if (*str_ptr == '\0') {
		parking_ptr->End (Round (MIDNIGHT));
	} else {
		parking_ptr->End (Round (time_step.Step (str_ptr)));
	}
	parking_ptr->Hourly (file->Hourly ());
	parking_ptr->Daily (file->Daily ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Parking Data");
	return (false);

link_error:
	return (Check_Message ("Parking Node %d is Not on Parking Link %d", lvalue, link_ptr->Link ()));
}
