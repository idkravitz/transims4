//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "RunSetup.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void RunSetup::Execute (void)
{
	int i, num;
	char batch [STRING_BUFFER], buffer [STRING_BUFFER];

	Setup_Group *setup_ptr;

	//---- start the processing loop ---

	if (!series_flag || setup_flag) {
		first_run = last_run = 0;
	}

	//---- create the run batch file ----

	if (file_prefix [0] != '\0') {
		str_fmt (buffer, sizeof (buffer), "%s%s", file_prefix, batch_name);
		str_cpy (batch_name, sizeof (batch_name), buffer);
	}

	if (first_run > 0) {
		if (last_run > first_run) {
			str_fmt (batch, sizeof (batch), "%s/%d-%d.%s", batch_dir, first_run, last_run, batch_name);
		} else {
			str_fmt (batch, sizeof (batch), "%s/%d.%s", batch_dir, first_run, batch_name);
		}
	} else {
		str_fmt (batch, sizeof (batch), "%s/%s", batch_dir, batch_name);
	}

	Show_Message (1, "Run Batch File = %s", batch);
	Print (1);
	batch_file.File_Type ("Run Batch File");
	batch_file.File_Access (Db_File::CREATE);
	if (!batch_file.Open (batch)) {
		File_Error (batch_file.File_Type (), batch_file.Filename ());
	}
	record = batch_file.Record ();
	size = batch_file.Max_Size ();

	//---- add the program path ----

#ifndef _WIN32
	exe [0] = '\0';
	str_cpy (message, sizeof (message), "#");
	str_cpy (record, size, "#!/bin/sh");
	batch_file.Write ();
	str_fmt (record, size, "export PATH=$PATH:%s", bin_dir);
#else
	str_cpy (exe, sizeof (exe), ".exe");
	str_cpy (message, sizeof (message), "rem");
	str_fmt (record, size, "path=%%PATH%%;%s", bin_dir);
#endif
	batch_file.Write ();

	*record = '\0';
	batch_file.Write ();

	Master_Processing ();

	//---- process setup control files ----

	if (setup_flag) {
		setup_flag = false;
		num = setup_group.Num_Records ();

		for (i=1; i <= num; i++) {
			setup_ptr = (Setup_Group *) setup_group [i];

			Write (2, "Processing Setup Control #%d = %s", setup_ptr->group_num, setup_ptr->setup_file);

			//---- clear the existing master control records ----

			control_group.Clear ();

			//---- read the setup file control keys ----

			if (!Read_Control_File (setup_ptr->setup_file)) {
				Error ("Reading Setup Control File #%d", setup_ptr->group_num);
			}
			Extension (setup_ptr->parameter);

			//---- process the master control keys ----

			Key_Processing (false);

			Master_Processing ();
		}
	}
	batch_file.Close ();

#ifndef _WIN32
	sprintf (buffer, "chmod 777 %s", batch);
	system (buffer);
#endif
	Exit_Stat (DONE);
}
