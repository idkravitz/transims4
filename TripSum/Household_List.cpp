//*********************************************************
//	Household_List.cpp - read the household list file
//*********************************************************

#include "TripSum.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Household_List
//---------------------------------------------------------

bool TripSum::Household_List (int file_num)
{
	int hhold;

	if (hhlist_file.Extend ()) {
		if (file_num > 0) {
			if (!hhlist_file.Open (file_num)) return (false);
			hhold_list.Reset ();
		}
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

		if (!hhold_list.Add (hhold)) {
			Error ("Adding Household %d to the List", hhold);
		}
	}
	End_Progress ();

	hhold = hhold_list.Num_Records ();
	total_hhold += hhold;

	if (hhlist_file.Extend ()) {
		Print (2, "Number of Household Records in %s = %d", hhlist_file.Extension (), hhold);
	} else {
		hhlist_file.Close ();
		Print (2, "Number of Household Records = %d", hhold);
	}
	hhold_list.Optimize ();
	return (true);
}
