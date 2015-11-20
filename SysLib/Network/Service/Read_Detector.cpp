//*********************************************************
//	Read_Detector.cpp - read the detector file
//*********************************************************

#include "Network_Service.hpp"

#include "Detector_File.hpp"

//---------------------------------------------------------
//	Read_Detector
//---------------------------------------------------------

void Network_Service::Read_Detector (void)
{
	Db_File *file = Network_Db_File (DETECTOR);

	//---- store the sign data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	if (Check_Data () || Renumber ()) { 
		if (!Network_File_Flag (NODE) && Network_Option_Flag (NODE)) {
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

		if (Detector_Processing (file)) {
			if (!detector_data.Add ()) {
				Error ("Adding Record to the Detector Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = detector_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Detector Data Records = %d", num);
	}
	detector_data.Optimize ();

	if (num > 0) Network_Data_True (DETECTOR);
}

//---------------------------------------------------------
//	Detector_Processing
//---------------------------------------------------------

bool Network_Service::Detector_Processing (Db_File *fh)
{
	int lvalue, index;
	char *svalue;
	bool ab_flag, node_flag, link_flag, coord_flag;

	Link_Data *link_ptr = NULL;
	Detector_Data *detector_ptr;

	Detector_File *file = (Detector_File *) fh;

	node_flag = Network_Data_Flag (NODE);
	link_flag = Network_Data_Flag (LINK);
	coord_flag = Network_Data_Flag (SIGNAL_COORDINATOR);

	//---- reserve memory ----

	if (detector_data.Max_Records () == 0) {
		if (!detector_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----
		
	detector_ptr = detector_data.New_Record ();
	if (detector_ptr == NULL) goto mem_error;

	//---- detector id ----

	lvalue = file->ID ();
	if (lvalue == 0) return (false);

	detector_ptr->ID (lvalue);

	lvalue = file->Link ();

	//---- check/convert the link number ----

	if (link_flag) {
		index = link_data.Get_Index (lvalue);

		if (index == 0) {
			Error ("Detector Link %d was Not Found in the Link file", lvalue);
		}
		link_ptr = link_data [index];

		detector_ptr->Link (Renumber () ? index : lvalue);
	} else {
		detector_ptr->Link (lvalue);
	}

	//---- determine the link direction ----

	lvalue = file->Node ();
	ab_flag = true;

	if (node_flag) {
		index = node_data.Get_Index (lvalue);

		if (index == 0) {
			return (Check_Message ("Detector Node %d was Not Found in the Node file", lvalue));
		}
		if (link_flag) {
			if (!Renumber ()) {
				index = lvalue;
			}
			if (link_ptr->Anode () == index) {
				ab_flag = false;
			} else if (link_ptr->Bnode () != index) {
				goto link_error;
			}
		}
	} else if (link_flag) {
		if (link_ptr->Anode () == lvalue) {
			ab_flag = false;
		} else if (link_ptr->Bnode () != lvalue) {
			goto link_error;
		}
	}

	//---- set the link dir value ----

	if (Renumber ()) {
		int dir = (ab_flag) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();
		if (dir == 0) {
			return (Check_Message ("Detector %d and Link %d are Incompatible", detector_ptr->ID (), link_ptr->Link ()));
		}
		detector_ptr->Link_Dir (dir);
	} else {
		detector_ptr->Dir ((ab_flag) ? 0 : 1);
	}

	//----- round the link offset ----

	detector_ptr->Offset (Round (file->Offset ()));	

	if (link_flag) {
		if (detector_ptr->Offset () < 0 || detector_ptr->Offset () > link_ptr->Length ()) {
			Warning ("Detector %d Offset %.1lf is Out of Range (0..%.1lf)",
				detector_ptr->ID (), UnRound (detector_ptr->Offset ()), UnRound (link_ptr->Length ()));

			if (detector_ptr->Offset () < 0) {
				detector_ptr->Offset (0);
			} else {
				detector_ptr->Offset (link_ptr->Length ());
			}
		}		
		if (node_flag && ab_flag) {		//---- offset from A node ----
			detector_ptr->Offset (link_ptr->Length () - detector_ptr->Offset ());
		}
	}

	//----- round the length ----

	detector_ptr->Length (Round (file->Length ()));

	//---- lane numbers ----

	if (link_flag) {
		int lanes;
		Dir_Data *dir_ptr;

		if (Renumber ()) {
			dir_ptr = dir_data [detector_ptr->Link_Dir ()];
		} else if (ab_flag) {
			dir_ptr = dir_data [link_ptr->AB_Dir ()];
		} else {
			dir_ptr = dir_data [link_ptr->BA_Dir ()];
		}
		if (dir_ptr == NULL) {
			return (Check_Message ("Link Direction on Detector Lane %d", detector_ptr->ID ()));
		}
		lanes = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right ();

		lvalue = file->Low_Lane ();

		if (lvalue < 0 || lvalue > lanes) goto lane_error;

		detector_ptr->Low_Lane (lvalue);

		lvalue = file->High_Lane ();

		if (lvalue < 0 || lvalue > lanes) goto lane_error;

		detector_ptr->High_Lane (lvalue);
	} else {
		detector_ptr->Low_Lane (file->Low_Lane ());
		detector_ptr->High_Lane (file->High_Lane ());
	}

	//----- optional fields ----

	svalue = file->Type ();

	if (*svalue != '\0') {
		detector_ptr->Type (Detector_Code (svalue));
	} else {
		detector_ptr->Type (0);
	}

	lvalue = file->Coordinator ();

	if (coord_flag && lvalue > 0) {
		index = coordinator_data.Get_Index (lvalue);

		if (index == 0) {
			Warning ("Detector Coordinator %d was Not Found in the Coordinator file", lvalue);
		}
		detector_ptr->Coordinator (Renumber () ? index : lvalue);
	} else {
		detector_ptr->Coordinator (lvalue);
	}

	return (true);

mem_error:
	Error ("Insufficient Memory for Detector Data");
	return (false);

link_error:
	return (Check_Message ("Detector Node %d is Not on Link %d", lvalue, link_ptr->Link ()));

lane_error:
	return (Check_Message ("Detector Lane %d is Out of Range for Link %d", lvalue, link_ptr->Link ()));
}
