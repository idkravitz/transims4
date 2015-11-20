//*********************************************************
//	Read_Pocket.cpp - read the pocket lane file
//*********************************************************

#include "Network_Service.hpp"

#include "Pocket_File.hpp"

//---------------------------------------------------------
//	Read_Pocket
//---------------------------------------------------------

void Network_Service::Read_Pocket (void)
{
	Pocket_File *file = (Pocket_File *) Network_Db_File (POCKET_LANE);

	//---- store the pocket data ----

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

		if (Pocket_Processing (file)) {
			if (!pocket_data.Add ()) {
				Error ("Adding Record to the Pocket Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = pocket_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Pocket Lane Data Records = %d", num);
	}
	pocket_data.Optimize ();

	if (num > 0) Network_Data_True (POCKET_LANE);
}

//---------------------------------------------------------
//	Pocket_Processing
//---------------------------------------------------------

bool Network_Service::Pocket_Processing (Db_File *fh)
{
	static bool dir_flag, lane_flag;
	
	bool node_flag, link_flag;
	int lvalue, index;

	Pocket_Data *pocket_ptr = NULL;
	Link_Data *link_ptr = NULL;

	Pocket_File *file = (Pocket_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (pocket_data.Max_Records () == 0) {
		dir_flag = (file->Field ("DIR") != NULL);
		lane_flag = (file->Field ("LANES") != NULL);

		if (!pocket_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	pocket_ptr = pocket_data.New_Record ();
	if (pocket_ptr == NULL) goto mem_error;

	lvalue = file->ID ();

	if (lvalue == 0) return (false);

	pocket_ptr->ID (lvalue);

	//---- check/convert the link number ----
	
	lvalue = file->Link ();

	if (link_flag) {
		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Pocket Link %d was Not Found in the Link file", lvalue));
		}
		link_ptr = link_data [index];

		pocket_ptr->Link (Renumber () ? index : lvalue);
	} else {
		pocket_ptr->Link (lvalue);
	}

	//---- determine the link direction ----

	lvalue = file->Node ();

	if (dir_flag) {
		pocket_ptr->Dir (lvalue);
	} else if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Pocket Node %d was Not Found in the Node file", lvalue));
		}
		if (link_flag) {
			if (!Renumber ()) {
				index = lvalue;
			}
			if (link_ptr->Anode () == index) {
				pocket_ptr->Dir (1);
			} else if (link_ptr->Bnode () == index) {
				pocket_ptr->Dir (0);
			} else {
				goto link_error;
			}
		} else {
			pocket_ptr->Dir (0);
		}
	} else if (link_flag) {
		if (link_ptr->Anode () == lvalue) {
			pocket_ptr->Dir (1);
		} else if (link_ptr->Bnode () == lvalue) {
			pocket_ptr->Dir (0);
		} else {
			goto link_error;
		}
	} else {
		pocket_ptr->Dir (0);
	}

	//---- lane number ----

	lvalue = file->Lane ();

	if (!lane_flag) {
		pocket_ptr->Lane (lvalue);
	} else if (link_flag) {		//***** lane ranges *****
		Dir_Data *dir_ptr;

		if (pocket_ptr->Dir ()) {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		}
		if (dir_ptr == NULL) {
			return (Check_Message ("Link Direction on Pocket Lane %d", pocket_ptr->ID ()));
		}
		if (lvalue < 0 || 
			(lvalue > dir_ptr->Left () && lvalue <= dir_ptr->Left () + dir_ptr->Thru ()) ||
			lvalue > dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ()) {

			return (Check_Message ("Pocket Lane %d is Out of Range for Link %d", lvalue, link_ptr->Link ()));
		}
		pocket_ptr->Lane (lvalue);
	} else {
		pocket_ptr->Lane (lvalue);
	}

	//---- set the pocket type ----
	
	pocket_ptr->Type (Pocket_Code (file->Type ()));

	//----- round the length ----

	pocket_ptr->Length (Round (file->Length ()));

	//----- round the pocket offset ----

	pocket_ptr->Offset (Round (file->Offset ()));

	if (link_flag) {
		if (pocket_ptr->Offset () < 0 || pocket_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Pocket Lane %d Offset %.1lf is Out of Range (0..%.1lf)",
				pocket_ptr->ID (), UnRound (pocket_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (pocket_ptr->Offset () < 0) {
				pocket_ptr->Offset (0);
			} else {
				pocket_ptr->Offset (link_ptr->Length ());
			}
		}		
		if (node_flag || dir_flag) {
			if (pocket_ptr->Offset () == 0) {
				if (pocket_ptr->Type () == TURN_LANE) {
					if (pocket_ptr->Dir () == 0) {
						pocket_ptr->Offset (pocket_ptr->Length () + link_ptr->Boffset ());
					} else {
						pocket_ptr->Offset (pocket_ptr->Length () + link_ptr->Aoffset ());
					}
				} else if (pocket_ptr->Type () == MERGE_LANE) {
					if (pocket_ptr->Dir () == 0) {
						pocket_ptr->Offset (link_ptr->Length () - link_ptr->Aoffset ());
					} else {
						pocket_ptr->Offset (link_ptr->Length () - link_ptr->Boffset ());
					}
				}
			}
			if (pocket_ptr->Dir () == 0) {		//---- offset from A node ----
				pocket_ptr->Offset (link_ptr->Length () - pocket_ptr->Offset ());
			}
		}
	}
	return (true);

mem_error:
	Error ("Insufficient Memory for Pocket Lane Data");
	return (false);

link_error:
	return (Check_Message ("Pocket Node %d is Not on Pocket Link %d", lvalue, link_ptr->Link ()));
}
