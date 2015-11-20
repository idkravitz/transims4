//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SmoothData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SmoothData::Execute (void)
{
	int n = 0;

	//---- process each group ----

	for (group_ptr = file_group.First (); group_ptr; group_ptr = file_group.Next ()) {

		if (n++) {
			Write (1);
			diurnal_data.Reset ();
		}
		Show_Message ("Processing Smooth Group %d -- Record", n);
		Set_Progress ();

		num_group = num_ini = num_in = num_out = 0;

		Read_Input ();

		End_Progress ();

		group_ptr->Input_File ()->Close ();
		group_ptr->Output_File ()->Close ();

		if (group_ptr->Initial_File () != NULL) {
			Write (1, "Number of %s Records = %d", group_ptr->Initial_File ()->File_Type (), num_ini);
			group_ptr->Initial_File ()->Close ();
		}
		Write (1, "Number of %s Records = %d", group_ptr->Input_File ()->File_Type (), num_in);
		Write (1, "Number of %s Records = %d", group_ptr->Output_File ()->File_Type (), num_out);

		if (num_group > 1) {
			Write (1, "Number of Smooth Groups within File #%d = %d", n, num_group);
		}
	}
	Exit_Stat (DONE);
}
