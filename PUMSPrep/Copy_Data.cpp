//*********************************************************
//	Copy_Data.cpp - copy the household and population files
//*********************************************************

#include "PUMSPrep.hpp"

//---------------------------------------------------------
//	Copy_Data
//---------------------------------------------------------

void PUMSPrep::Copy_Data (void)
{
	//---- copy the household file ----

	Show_Message ("Reading %s -- Record", hh_file.File_Type ());
	Set_Progress ();

	while (hh_file.Read ()) {
		Show_Progress ();
		hh_in++;

		new_hh_file.Copy_Fields (&hh_file);

		if (!new_hh_file.Write ()) {
			Error ("Writing %s", new_hh_file.File_Type ());
		}
		hh_out++;
	}
	End_Progress ();

	hh_file.Close ();

	//---- copy the population file ----

	Show_Message ("Reading %s -- Record", pop_file.File_Type ());
	Set_Progress ();

	while (pop_file.Read ()) {
		Show_Progress ();
		pop_in++;

		new_pop_file.Copy_Fields (&pop_file);

		if (!new_pop_file.Write ()) {
			Error ("Writing %s", new_pop_file.File_Type ());
		}
		pop_out++;
	}
	End_Progress ();

	pop_file.Close ();
}
