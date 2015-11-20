//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "RunSetup.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void RunSetup::Program_Control (void)
{
	int i, num_group;
	char *str_ptr, buffer [STRING_BUFFER];

	Db_File setup_file;
	Setup_Group setup;

	//---- check for setup files ----

	num_group = Highest_Control_Group (SETUP_CONTROL_FILE_x, 0);

	setup_flag = (num_group > 0);

	//---- process the master control keys ----

	Key_Processing (true);

	//---- process each setup group ----

	for (i=1; i <= num_group; i++) {

		//---- get the setup control file ----

		str_ptr = Get_Control_String (SETUP_CONTROL_FILE_x, i);
		if (str_ptr == NULL) continue;

		Print (1);
		setup.group_num = i;

		//---- open setup control file ----

		str_fmt (buffer, sizeof (buffer), "Setup Control File #%d", i);
		setup_file.File_Type (buffer);

		if (!setup_file.Open (str_ptr)) {
			File_Error (setup_file.File_Type (), setup_file.Filename ());
		}
		str_cpy (setup.setup_file, sizeof (setup.setup_file), setup_file.Filename ());
		setup_file.Close ();

		//---- get the setup parameter ----

		str_ptr = Get_Control_String (SETUP_PARAMETER_x, i);

		if (str_ptr == NULL) {
			setup.parameter [0] = '\0';
		} else {
			str_cpy (setup.parameter, sizeof (setup.parameter), str_ptr);

			Print (1, "Setup Parameter #%d = %s", i, str_ptr);
		}

		//---- add the files to the group ----

		if (!setup_group.Add (&setup)) goto mem_error;
	}
	return;

mem_error:
	Error ("Insufficient Memory for Control Files");
} 
