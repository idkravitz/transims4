//*********************************************************
//	Read_Location.cpp - read the activity location file
//*********************************************************

#include "Network_Service.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Read_Location
//---------------------------------------------------------

void Network_Service::Read_Location (void)
{
	Location_File *file = (Location_File *) Network_Db_File (ACTIVITY_LOCATION);

	if (file == NULL) {
		Error ("Activity Location File is Not Open");
	}

	//---- store the location data ----

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

		if (Location_Processing (file)) {
			if (!location_data.Add ()) {
				Error ("Adding Record to the Activity Location Data List");			
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = location_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Activity Location Data Records = %d", num);
	}
	location_data.Optimize ();

	if (num > 0) Network_Data_True (ACTIVITY_LOCATION);
}

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool Network_Service::Location_Processing (Db_File *fh)
{
	static bool dir_flag;

	bool node_flag, link_flag;
	int lvalue, index;

	Link_Data *link_ptr = NULL;	
	Location_Data *location_ptr = NULL;

	Location_File *file = (Location_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);

	//---- reserve memory ----

	if (location_data.Max_Records () == 0) {
		dir_flag = (file->Field ("DIR") != NULL);

		if (!location_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
		
	location_ptr = location_data.New_Record ();
	if (location_ptr == NULL) goto mem_error;

	location_ptr->Location (file->Location ());

	if (location_ptr->Location () == 0) return (false);

	//---- check/convert the link number ----
	
	lvalue = file->Link ();

	if (link_flag) {
		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Activity Location Link %d was Not Found in the Link file", lvalue));
		}
		link_ptr = link_data [index];

		location_ptr->Link (Renumber () ? index : lvalue);
	} else {
		location_ptr->Link (lvalue);
	}

	//---- determine the link direction ----

	lvalue = file->Node ();

	if (dir_flag) {
		location_ptr->Dir (lvalue);
	} else if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Activity Location Node %d was Not Found in the Node file", lvalue));
		}
		if (link_flag) {
			if (!Renumber ()) {
				index = lvalue;
			}
			if (link_ptr->Anode () == index) {
				location_ptr->Dir (1);
			} else if (link_ptr->Bnode () == index) {
				location_ptr->Dir (0);
			} else {
				goto link_error;
			}
		} else {
			location_ptr->Dir (0);
		}
	} else if (link_flag) {
		if (link_ptr->Anode () == lvalue) {
			location_ptr->Dir (1);
		} else if (link_ptr->Bnode () == lvalue) {
			location_ptr->Dir (0);
		} else {
			goto link_error;
		}
	} else {
		location_ptr->Dir (0);
	}

	//----- round the link offset ----

	location_ptr->Offset (Round (file->Offset ()));

	if (link_flag) {
		if (location_ptr->Offset () < 0 || location_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Activity Location %d Offset %.1lf is Out of Range (0..%.1lf)",
				location_ptr->ID (), UnRound (location_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (location_ptr->Offset () < 0) {
				location_ptr->Offset (0);
			} else {
				location_ptr->Offset (link_ptr->Length ());
			}
		}
		if (node_flag || dir_flag) {
			if (location_ptr->Dir () == 0) {	//---- offset from A node ----
				location_ptr->Offset (link_ptr->Length () - location_ptr->Offset ());
			}
		}
	}
	location_ptr->X (Round (file->X ()));
	location_ptr->Y (Round (file->Y ()));

	//----- optional fields ----

	location_ptr->Zone (file->Zone ());

	return (true);

mem_error:
	Error ("Insufficient Memory for Activity Location Data");
	return (false);

link_error:
	return (Check_Message ("Activity Location Node %d is Not on Link %d", lvalue, link_ptr->Link ()));
}

