//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "HHList.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void HHList::Execute (void)
{
	int i, hhold, last_hhold, num_out, num_in;
	Db_File *file;

	Show_Message ("Reading %s -- Record", input_file.File_Type ());
	Set_Progress ();

	num_in = num_out = last_hhold = 0;

	for (;;) {

		//---- read the first field ----

		if (list_flag) {
			if (!list_file.Read ()) break;
			Get_Integer (list_file.Record (), &hhold);
		} else {
			if (!input_file.Read ()) break;
			input_file.Get_Field (1, &hhold);
		}
		Show_Progress ();
		num_in++;

		if (hhold == 0 || hhold == last_hhold) continue;

		last_hhold = hhold;

		i = (int) (num_files * random.Probability ());
		if (i >= num_files) i = num_files - 1;

		file = output_file [i];

		str_fmt (file->Record (), file->Max_Size (), "%d", hhold);

		if (!file->Write ()) {
			Error ("Writing Output File");
		}
		num_out++;
	}
	End_Progress ();

	if (list_flag) {
		list_file.Close ();
	} else {
		input_file.Close ();
	}

	//---- read the second file ----

	if (two_flag) {
		Show_Message ("Reading %s -- Record", input2_file.File_Type ());
		Set_Progress ();

		while (input2_file.Read ()) {
			input2_file.Get_Field (1, &hhold);

			Show_Progress ();
			num_in++;

			if (hhold == 0 || hhold == last_hhold) continue;

			last_hhold = hhold;

			i = (int) (num_files * random.Probability ());
			if (i >= num_files) i = num_files - 1;

			file = output_file [i];

			str_fmt (file->Record (), file->Max_Size (), "%d", hhold);

			if (!file->Write ()) {
				Error ("Writing Output File");
			}
			num_out++;
		}
		End_Progress ();
		input2_file.Close ();
	}

	for (i=0; i < num_files; i++) {
		output_file [i]->Close ();
	}
	Write (2, "Number of Input Data Records  = %d", num_in);
	Write (1, "Number of Output Data Records = %d", num_out);

	Exit_Stat (DONE);
}
