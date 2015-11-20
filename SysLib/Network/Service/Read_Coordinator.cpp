//*********************************************************
//	Read_Coordinator.cpp - read the coordinator file
//*********************************************************

#include "Network_Service.hpp"

#include "Coordinator_File.hpp"

//---------------------------------------------------------
//	Read_Coordinator
//---------------------------------------------------------

void Network_Service::Read_Coordinator (void)
{
	Db_File *file = Network_Db_File (SIGNAL_COORDINATOR);

	//---- store the node data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();
	
	file->Num_Records ();
	First_Record (true);

	while (file->Read ()) {
		Show_Progress ();

		if (Coordinator_Processing (file)) {
			if (!coordinator_data.Add ()) {
				Error ("Adding Record to the Signal Coordinator Data List");
			}
		}
		First_Record (false);
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	int num = coordinator_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Signal Coordinator Data Records = %d", num);
	}
	coordinator_data.Optimize ();

	if (num > 0) Network_Data_True (SIGNAL_COORDINATOR);
}

//---------------------------------------------------------
//	Coordinator_Processing
//---------------------------------------------------------

bool Network_Service::Coordinator_Processing (Db_File *fh)
{
	Coordinator_Data *coord_ptr;

	Coordinator_File *file = (Coordinator_File *) fh;

	//---- reserve memory ----

	if (coordinator_data.Max_Records () == 0) {
		if (!coordinator_data.Max_Records (file->Num_Records ())) goto mem_error;
	}

	//---- read and save the data ----

	coord_ptr = coordinator_data.New_Record ();
	if (coord_ptr == NULL) goto mem_error;

	coord_ptr->ID (file->ID ());
	if (coord_ptr->ID () == 0) return (false);

	return (true);

mem_error:
	Error ("Insufficient Memory for Signal Coordinator Data");
	return (false);
}
