//*********************************************************
//	Zone_Process.cpp - Read the Zone File
//*********************************************************

#include "TransimsNet.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool TransimsNet::Zone_Processing (Db_File *fh)
{
	if (Network_Service::Zone_Processing (fh)) {
		if (zout_flag) {
			Zone_File *file = (Zone_File *) fh;
			Zone_File *new_file = (Zone_File *) Network_Db_File (NEW_ZONE);

			new_file->Copy_Fields (file);

			if (Projection_Flag ()) {
				Zone_Data *zone_ptr = zone_data.New_Record ();

				new_file->X (UnRound (zone_ptr->X ()));
				new_file->Y (UnRound (zone_ptr->Y ()));
			}
			if (!new_file->Write ()) {
				Error ("Writing Zone Number %d", new_file->Zone ());
			}
			nzout++;
		}
		return (true);
	}
	return (false);
}
