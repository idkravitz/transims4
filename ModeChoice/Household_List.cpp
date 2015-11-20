//*********************************************************
//	Household_List.cpp - read the household list file
//*********************************************************

#include "ModeChoice.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Household_List
//---------------------------------------------------------

void ModeChoice::Household_List (void)
{
	int hhold, nfile;

	for (nfile=0; ; nfile++) {
		if (hhlist_file.Extend ()) {
			if (nfile > 0) {
				if (!hhlist_file.Open (nfile)) return;
			}
			Show_Message ("Reading %s %s -- Record", hhlist_file.File_Type (), hhlist_file.Extension ());
		} else {
			if (nfile > 0) return;
			Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
		}
		Set_Progress ();

		//---- store the household list ----

		while (hhlist_file.Read ()) {
			Show_Progress ();

			Get_Integer (hhlist_file.Record (), &hhold);

			if (hhold <= 0) continue;

			if (!hhold_list.Add (hhold)) {
				Error ("Adding Household %d to the List", hhold);
			}
		}
		End_Progress ();
	}
	hhlist_file.Close ();

	Print (2, "Total Number of Household List Records = %d", hhold_list.Num_Records ());

	hhold_list.Optimize ();
}
