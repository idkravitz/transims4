//*********************************************************
//	Read_Deletes.cpp - read the delete node control file
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Deletes
//---------------------------------------------------------

void IntControl::Read_Deletes (void)
{
	int node;

	//---- store the delete data ----

	Show_Message ("Reading %s -- Record", delete_file.File_Type ());
	Set_Progress ();

	while (delete_file.Read ()) {
		Show_Progress ();

		Get_Integer (delete_file.Record (), &node);
		if (node == 0) continue;

		if (!delete_node.Add (node)) {
			Error ("Adding Delete Node Record");
		}
	}
	End_Progress ();

	delete_file.Close ();

	Print (2, "Number of %s Records = %d", delete_file.File_Type (), Progress_Count ());
}
