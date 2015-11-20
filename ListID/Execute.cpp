//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ListID.hpp"

#include "Integer_Array.hpp"
#include "Complex_Array.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ListID::Execute (void)
{
	int num, nrec;

	Integer_Sort list;

	Show_Message ("Reading %s -- Record", input_file.File_Type ());
	Set_Progress ();

	while (input_file.Read ()) {
		Show_Progress ();

		//---- skip the header records ----

		if (Progress_Count () <= num_head) continue;

		//---- read the field ----

		Get_Integer (input_file.Record (), &num);

		if (num > 0) {
			list.Add (num);
		}
	}
	End_Progress ();
	nrec = Progress_Count ();

	input_file.Close ();

	Show_Message ("Writing %s -- Record", output_file.File_Type ());
	Set_Progress ();

	for (num = list.First_Key (); num; num = list.Next_Key ()) {
		Show_Progress ();

		str_fmt (output_file.Record (), output_file.Max_Size (), "%d", num);

		if (!output_file.Write ()) {
			Error ("Writing Output File");
		}
	}
	End_Progress ();

	output_file.Close ();

	Write (2, "Number of Input Data Records  = %d", nrec);
	Write (1, "Number of Output ID Records   = %d", Progress_Count ());

	Exit_Stat (DONE);
}
