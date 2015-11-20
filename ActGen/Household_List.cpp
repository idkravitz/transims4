//*********************************************************
//	Household_List.cpp - read the household list file
//*********************************************************

#include "ActGen.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Household_List
//---------------------------------------------------------

void ActGen::Household_List (void)
{
	int hhold;

	Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
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

	hhlist_file.Close ();

	nhh_list = hhold_list.Num_Records ();

	Print (2, "Number of Household List Records = %d", nhh_list);

	hhold_list.Optimize ();
}
