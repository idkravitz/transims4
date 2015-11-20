//*********************************************************
//	Copy_Data.cpp - copy the zone data file
//*********************************************************

#include "SF3Prep.hpp"

//---------------------------------------------------------
//	Copy_Data
//---------------------------------------------------------

void SF3Prep::Copy_Data (void)
{
	//---- copy the zone data file ----

	Show_Message ("Reading %s -- Record", zone_file.File_Type ());
	Set_Progress ();

	while (zone_file.Read ()) {
		Show_Progress ();
		zone_in++;

		new_zone_file.Copy_Fields (&zone_file);

		if (!new_zone_file.Write ()) {
			Error ("Writing %s", new_zone_file.File_Type ());
		}
		zone_out++;
	}
	End_Progress ();

	zone_file.Close ();
}
