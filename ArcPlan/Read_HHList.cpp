//*********************************************************
//	Read_HHList.cpp - Read the Household List Files
//*********************************************************

#include "ArcPlan.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Read_HHList
//---------------------------------------------------------

void ArcPlan::Read_HHList (void)
{
	int nfile, hhold;

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

			if (!hhold_list.Add (hhold)) {
				Error ("Adding Household %d", hhold);
			}
		}
		End_Progress ();

		hhlist_file.Close ();
	}
	hhold = hhold_list.Num_Records ();

	hhold_list.Max_Records (hhold);
	
	Print (2, "Number of Household List Records = %d", hhold);
}
