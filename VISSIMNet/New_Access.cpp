//*********************************************************
//	New_Access.cpp - write the new process link records
//*********************************************************

#include "VISSIMNet.hpp"

#include "Access_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	New_Access
//---------------------------------------------------------

void VISSIMNet::New_Access (void)
{
	Access_Data *access_ptr;
	Access_File *access_file;

	//---- copy and renumber the process links ----

	for (access_ptr = access_data.First (); access_ptr; access_ptr = access_data.Next ()) {

		if (access_ptr->From_Type () != PARKING_ID && access_ptr->To_Type () != PARKING_ID) continue;

		//---- save the process link ----

		if (!new_access_data.Add (access_ptr)) {
			Error ("Adding New Process Link Data");
		}
	}

	//---- write the new access file ----

	if (!Network_File_Flag (NEW_PROCESS_LINK)) return;

	access_file = (Access_File *) Network_Db_Base (NEW_PROCESS_LINK);

	for (access_ptr = new_access_data.First (); access_ptr; access_ptr = new_access_data.Next ()) {

		access_file->ID (access_ptr->ID ());
		access_file->From_ID (access_ptr->From_ID ());
		access_file->From_Type (End_Code ((End_Type) access_ptr->From_Type ()));
		access_file->To_ID (access_ptr->To_ID ());
		access_file->To_Type (End_Code ((End_Type) access_ptr->To_Type ()));
		access_file->Time (UnRound (access_ptr->Time ()));
		access_file->Cost (access_ptr->Cost ());

		if (!access_file->Write ()) {
			Error ("Writing %s", access_file->File_Type ());
		}
	}
	access_file->Close ();

	Print (2, "Number of New Process Link Records = %d", new_access_data.Num_Records ());
}
