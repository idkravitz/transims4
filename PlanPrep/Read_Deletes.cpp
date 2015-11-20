//*********************************************************
//	Read_Deletes.cpp - Read the delete household list files
//*********************************************************

#include "PlanPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Read_Deletes
//---------------------------------------------------------

void PlanPrep::Read_Deletes (void)
{
	int hhold;

	for (nout=0; ; nout++) {

		//---- open the delete household list plans ----

		if (!delete_file.Open (nout)) break;

		if (delete_file.Extend ()) {
			Show_Message ("Reading %s %s -- Record", delete_file.File_Type (), delete_file.Extension ());
		} else {
			Show_Message ("Reading %s -- Record", delete_file.File_Type ());
		}
		Set_Progress (100000);

		//---- read the household list file ----

		while (delete_file.Read ()) {
			Show_Progress ();

			Get_Integer (delete_file.Record (), &hhold);

			if (!delete_list.Add (hhold)) {
				Error ("Adding Delete Household Record");
			}
		}
		End_Progress ();

		delete_file.Close ();
	}
	if (nout == 0) {
		File_Error ("No Delete Household List Files were Found", delete_file.Filename ());
	}
	nout = 0;
}
