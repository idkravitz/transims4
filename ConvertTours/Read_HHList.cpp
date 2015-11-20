//*********************************************************
//	Read_HHList.cpp - Read the household list files
//*********************************************************

#include "ConvertTours.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_HHList
//---------------------------------------------------------

void ConvertTours::Read_HHList (void)
{
	int hhold;

	Show_Message ("Reading %s -- Record", hhlist_file.File_Type ());
	Set_Progress (100000);

	//---- read the household list file ----

	while (hhlist_file.Read ()) {
		Show_Progress ();

		Get_Integer (hhlist_file.Record (), &hhold);

		if (!hhold_list.Add (hhold)) {
			Error ("Adding Household List Record");
		}
		if (hhold > max_hhlist) max_hhlist = hhold;
	}
	End_Progress ();

	hhlist_file.Close ();
}
