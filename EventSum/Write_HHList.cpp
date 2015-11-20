//*********************************************************
//	Write_HHList.cpp - Write the Household List Files
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_HHList
//---------------------------------------------------------

void EventSum::Write_HHList (void)
{
	int i, num_out;
	HHList_Data *hhlist_ptr;

	FILE *file;

	//---- write the household list files ----

	num_out = 0;

	for (i=0; i < nfile; i++) {
		if (!new_hhlist_file.Open (i)) {
			File_Error (new_hhlist_file.File_Type (), new_hhlist_file.Filename ());
		}
		if (new_hhlist_file.Extend ()) {
			Show_Message ("Writing %s %s -- Record", new_hhlist_file.File_Type (), new_hhlist_file.Extension ());
		} else {
			Show_Message ("Writing %s -- Record", new_hhlist_file.File_Type ());
		}
		Set_Progress ();

		file = new_hhlist_file.File ();

		for (hhlist_ptr = hhlist_data.First_Key (); hhlist_ptr; hhlist_ptr = hhlist_data.Next_Key ()) {
			if (hhlist_ptr->File () == i) {
				Set_Progress ();

				if (hhlist_ptr->Keep ()) {
					fprintf (file, "%d\n", hhlist_ptr->Household ());
					num_out++;
				}
			}
		}
		End_Progress ();

		new_hhlist_file.Close ();
	}
	i = hhlist_data.Num_Records ();
	if (i < 1) i = 1;

	Print (2, "Number of %s Records = %d (%.1lf%%)", new_hhlist_file.File_Type (), 
		num_out, (100.0 * num_out / i));
}
