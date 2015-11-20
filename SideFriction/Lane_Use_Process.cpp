//*********************************************************
//	Lane_Use_Process.cpp - additional lane use processing
//*********************************************************

#include "SideFriction.hpp"

//---------------------------------------------------------
//	Lane_Use_Processing
//---------------------------------------------------------

bool SideFriction::Lane_Use_Processing (Db_File *fh)
{
	Lane_Use_Data *lane_use_ptr;

	Lane_Use_File *file = (Lane_Use_File *) fh;

	if (Network_Service::Lane_Use_Processing (file)) {
		new_file->Copy_Fields (file);

		lane_use_ptr = lane_use_data.New_Record ();

		new_file->Link (lane_use_ptr->Link ());
		new_file->Dir (lane_use_ptr->Dir ());
		new_file->Lane (lane_use_ptr->Lane ());
		new_file->Use (Use_Code (lane_use_ptr->Use ()));
		new_file->Type (Restrict_Code ((Restrict_Type) lane_use_ptr->Type ()));
		new_file->Start (time_step.Format_Step (Resolve (lane_use_ptr->Start ())));
		new_file->End (time_step.Format_Step (Resolve (lane_use_ptr->End ())));
		new_file->Offset (UnRound (lane_use_ptr->Offset ()));
		new_file->Length (UnRound (lane_use_ptr->Length ()));

		if (!new_file->Write ()) {
			Error ("Writing %s", new_file->File_Type ());
		}
		num_use++;
		return (true);
	}
	return (false);
}

