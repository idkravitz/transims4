//*********************************************************
//	Read_HHList.cpp - Read the Household List Files
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_HHList
//---------------------------------------------------------

void EventSum::Read_HHList (void)
{
	int hhold;
	HHList_Data *hhlist_ptr;

	//---- read the household list files ----

	for (nfile=0; ; nfile++) {
		if (!hhlist_file.Open (nfile)) break;

		if (hhlist_file.Extend ()) {
			Show_Message ("Reading %s %s -- Record", hhlist_file.File_Type (), hhlist_file.Extension ());
		} else {
			Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
		}
		Set_Progress ();

		//---- store the household list ----

		while (hhlist_file.Read ()) {
			Show_Progress ();

			Get_Integer (hhlist_file.Record (), &hhold);

			if (hhold <= 0) continue;
			if (hhold > max_hhold) {
				Error ("Household %d is Out of Range (1..%d)", hhold, max_hhold);
			}
			if (range_flag) {
				if (!hhold_range.In_Range (hhold)) continue;
			}
			hhlist_ptr = hhlist_data.Get (hhold);

			if (hhlist_ptr != NULL) {
				Warning ("Household %d was Found in Files %s and %s", hhold, 
					hhlist_file.Extension (hhlist_ptr->File ()), hhlist_file.File_Type ());
				continue;
			}

			//---- save a new household ----

			hhlist_ptr  = hhlist_data.New_Record ();
			if (hhlist_ptr == NULL) goto mem_error;

			hhlist_ptr->Household (hhold);
			hhlist_ptr->File (nfile);
			hhlist_ptr->Keep (false);

			if (!hhlist_data.Add ()) goto mem_error;
		}
		End_Progress ();

		hhlist_file.Close ();
	}
	hhold = hhlist_data.Num_Records ();

	hhlist_data.Max_Records (hhold);
	
	Print (2, "Number of Household List Files = %d", nfile);
	Print (1, "Number of Household List Records = %d", hhold);
	return;

mem_error:
	Error ("Insufficient Memory for Household List Data");
}
