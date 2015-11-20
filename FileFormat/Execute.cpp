//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void FileFormat::Execute (void)
{
	//---- process each file ----

	for (int i=0; i < nfiles; i++) {
		if (input_file [i] == NULL) continue;

		Show_Message ("Converting %s", input_file [i]->File_Type ());
		Set_Progress ();

		if (input_file [i]->Nested ()) {
			int count, field;

			field = nest_field [i];

			while (input_file [i]->Read (false)) {
				Show_Progress ();

				output_file [i]->Copy_Fields (input_file [i]);
				output_file [i]->Write (false);

				input_file [i]->Get_Field (field, &count);

				while (count-- > 0) {
					if (!input_file [i]->Read (true)) {
						Error ("Reading Nested Record");
					}
					Show_Progress ();

					output_file [i]->Copy_Fields (input_file [i]);
					output_file [i]->Write (true);
				}
			}
		} else {
			while (input_file [i]->Read ()) {
				Show_Progress ();

				output_file [i]->Copy_Fields (input_file [i]);
				output_file [i]->Write ();
			}
		}
		End_Progress ();
		input_file [i]->Close ();
		output_file [i]->Close ();

		Print (1);
		Write (1, "Number of %s Records Converted = %d", input_file [i]->File_Type (), Progress_Count ());
		Show_Message (1);
	}

	Exit_Stat (DONE);
}
