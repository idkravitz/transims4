//*********************************************************
//	Write_Activity.cpp - Write the Link Activity File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Activity
//---------------------------------------------------------

void LinkSum::Write_Activity (void)
{
	Show_Message ("Writing Link Activity File -- Record");
	Set_Progress ();

	link_db.Rewind ();

	while (link_db.Read_Record ()) {
		Show_Progress ();

		activity_file.Copy_Fields (&link_db);

		if (!activity_file.Write ()) {
			Error ("Writing the Link Activity File");
		}
	}
	End_Progress ();

	Print (2, "Number of Link Activity Records = %d", Progress_Count ());

	activity_file.Close ();
}
