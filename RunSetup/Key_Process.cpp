//*********************************************************
//	Key_Processing.cpp - additional control key processing
//*********************************************************

#include "RunSetup.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Key_Processing
//---------------------------------------------------------

void RunSetup::Key_Processing (bool batch_flag)
{
	int i, j, num_group, len;
	char *str_ptr, *ptr, buffer [STRING_BUFFER], param0 [4] [FIELD_BUFFER], dir0 [FIELD_BUFFER];
	char run0 [FIELD_BUFFER], previous0 [FIELD_BUFFER], append0 [4] [FIELD_BUFFER];
	bool comment_flag0 [4], flag0 [4];

	Control_Group group;

	//---- check for a series control ----

	str_ptr = Extension ();
	series_flag = (*str_ptr != '\0');

	if (series_flag) {
		str_cpy (buffer, sizeof (buffer), str_ptr);

		str_ptr = strchr (buffer, '.');

		if (str_ptr != NULL) {
			*str_ptr++ = '\0';
			while (*str_ptr == '.') str_ptr++;

			first_run = atoi (buffer);
			last_run = atoi (str_ptr);

			if (first_run < 1 || last_run < first_run) {
				Error ("Command Line Parameter %s is Out of Range", Extension ());
			}
		} else {
			first_run = last_run = atoi (buffer);
		}
	}

	//---- get the program directory ----

	str_ptr = Get_Control_String (PROGRAM_DIRECTORY);

	if (str_ptr != NULL) {
		Print_Filename (2, "Program Directory", str_ptr);

		str_cpy (bin_dir, sizeof (bin_dir), str_ptr);
	} else {
		bin_dir [0] = '\0';
	}
	
	//---- get the control directory ----

	str_ptr = Get_Control_String (CONTROL_DIRECTORY);

	if (str_ptr == NULL) {
		str_ptr = "control";
	}
	ptr = strchr (str_ptr, '/');
	if (ptr == NULL) {
		ptr = strchr (str_ptr, '\\');
	}
	if (ptr == NULL) {
		str_cpy (buffer, sizeof (buffer), Project_Filename (str_ptr));
	} else {
		str_cpy (buffer, sizeof (buffer), str_ptr);
	}
	str_cpy (control_dir, sizeof (control_dir), buffer);

	Print_Filename (2, "Control Directory", control_dir);

	//---- get the network directory ----

	str_ptr = Get_Control_String (NETWORK_DIRECTORY);

	if (str_ptr == NULL) {
		str_ptr = "network";
	}
	ptr = strchr (str_ptr, '/');
	if (ptr == NULL) {
		ptr = strchr (str_ptr, '\\');
	}
	if (ptr == NULL) {
		str_cpy (buffer, sizeof (buffer), Project_Filename (str_ptr));
	} else {
		str_cpy (buffer, sizeof (buffer), str_ptr);
	}
	str_cpy (network_dir, sizeof (network_dir), buffer);

	Print_Filename (2, "Network Directory", network_dir);

	if (batch_flag) {

		//---- get the batch directory ----

		str_ptr = Get_Control_String (BATCH_DIRECTORY);

		if (str_ptr == NULL) {
			str_ptr = control_dir;
		}
		ptr = strchr (str_ptr, '/');
		if (ptr == NULL) {
			ptr = strchr (str_ptr, '\\');
		}
		if (ptr == NULL) {
			str_cpy (buffer, sizeof (buffer), Project_Filename (str_ptr));
		} else {
			str_cpy (buffer, sizeof (buffer), str_ptr);
		}
		str_cpy (batch_dir, sizeof (batch_dir), buffer);

		Print_Filename (2, "Batch Directory", batch_dir);

		//---- get the batch name ----

		str_ptr = Get_Control_String (BATCH_NAME);

		if (str_ptr == NULL) {
			str_ptr = "RunAll";
		}
		str_cpy (batch_name, sizeof (batch_name), str_ptr);

		//---- check the extension ----

		len = (int) strlen (batch_name);
		ptr = strchr (batch_name + (len - 4), '.');
		if (ptr == NULL) {
#ifndef _WIN32
			str_cat (batch_name, sizeof (batch_name), ".sh");
#else
			str_cat (batch_name, sizeof (batch_name), ".bat");
#endif
		}
		Print (2, "Batch Name = %s", batch_name);
	}

	//---- get the program flags ----

	str_ptr = Get_Control_String (PROGRAM_FLAGS);

	if (str_ptr != NULL) {
		str_cpy (program_flags, sizeof (program_flags), str_ptr);

		Print (2, "Program Flags = %s", str_ptr);
	} else {
		program_flags [0] = '\0';
	}

	//---- get the exit check ----

	str_ptr = Get_Control_String (EXIT_CHECK);

	if (str_ptr == NULL) {
#ifndef _WIN32
		str_ptr = "if [ \"$?\" == \"1\" ]; then exit 1; fi";
#else
		str_ptr = "if %ERRORLEVEL% == 1 exit 1";
#endif
	}
	str_cpy (exit_check, sizeof (exit_check), str_ptr);

	Print (2, "Exit Check = %s", str_ptr);

	//---- get the default directory ----

	str_ptr = Get_Control_String (DIRECTORY);

	if (str_ptr != NULL) {
		str_cpy (dir0, sizeof (dir0), str_ptr);
	} else {
		dir0 [0] = '\0';
	}

	//---- get the file format ----

	str_ptr = Get_Control_String (FILE_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "TAB_DELIMITED";
	}
	str_cpy (file_format, sizeof (file_format), str_ptr);

	Print (2, "File Format = %s", str_ptr);

	//---- get the model name ----

	str_ptr = Get_Control_String (MODEL_NAME);

	if (str_ptr != NULL) {
		str_cpy (model_name, sizeof (model_name), str_ptr);

		Print (2, "Model Name = %s", str_ptr);
	} else {
		model_name [0] = '\0';
	}
	//---- get the alternative name ----

	str_ptr = Get_Control_String (ALTERNATIVE_NAME);

	if (str_ptr != NULL) {
		str_cpy (alt_name, sizeof (alt_name), str_ptr);

		Print (2, "Alternative Name = %s", str_ptr);
	} else {
		alt_name [0] = '\0';
	}

	//---- get the analysis year ----

	str_ptr = Get_Control_String (ANALYSIS_YEAR);

	if (str_ptr != NULL) {
		str_cpy (year_name, sizeof (year_name), str_ptr);

		Print (2, "Analysis Year = %s", str_ptr);
	} else {
		year_name [0] = '\0';
	}

	//---- build the file prefix ----

	if (alt_name [0] != '\0') {
		str_fmt (file_prefix, sizeof (file_prefix), "%s.", alt_name);
	} else {
		file_prefix [0] = '\0';
	}
	if (year_name [0] != '\0') {
		str_cat (file_prefix, sizeof (file_prefix), year_name);
		str_cat (file_prefix, sizeof (file_prefix), ".");
	}
	if (model_name [0] != '\0') {
		str_cat (file_prefix, sizeof (file_prefix), model_name);
		str_cat (file_prefix, sizeof (file_prefix), ".");
	}

	//---- get the default run name ----

	if (series_flag) {
		str_fmt (run0, sizeof (run0), "%d", first_run);
	} else {
		str_ptr = Get_Control_String (RUN_NAME);

		if (str_ptr != NULL) {
			str_cpy (run0, sizeof (run0), str_ptr);
		} else {
			run0 [0] = '\0';
		}
	}

	//---- get the default previous run name ----

	if (series_flag && first_run > 1) {
		str_fmt (previous0, sizeof (previous0), "%d", first_run - 1);
	} else {
		str_ptr = Get_Control_String (PREVIOUS_RUN_NAME);

		if (str_ptr != NULL) {
			str_cpy (previous0, sizeof (previous0), str_ptr);
		} else {
			previous0 [0] = '\0';
		}
	}

	//---- get the default parameters ----

	for (j=0; j < 4; j++) {
		str_ptr = Get_Control_String (param [j]);

		if (str_ptr != NULL) {
			str_cpy (param0 [j], sizeof (param0 [j]), str_ptr);
		} else {
			param0 [j][0] = '\0';
		}
	}

	//---- get the default append keys ----

	for (j=0; j < 4; j++) {
		str_ptr = Get_Control_String (append [j]);

		if (str_ptr != NULL) {
			str_cpy (append0 [j], sizeof (append0 [j]), str_ptr);
		} else {
			append0 [j][0] = '\0';
		}
	}

	//---- get the default comment flags ----

	for (j=0; j < 4; j++) {
		str_ptr = Get_Control_String (comment [j]);

		if (str_ptr != NULL) {
			comment_flag0 [j] = Get_Control_Flag (comment [j]);
			flag0 [j] = true;
		} else {
			flag0 [j] = false;
		}
	}

	//---- get the number of groups ----

	num_group = Highest_Control_Group (MASTER_CONTROL_FILE_x, 0);

	if (!control_group.Max_Records (num_group + 1)) goto mem_error;

	//---- process each group ----

	for (i=0; i <= num_group; i++) {

		//---- get the master control file ----

		if (i == 0) {
			str_ptr = Get_Control_String (MASTER_CONTROL_FILE);
			if (str_ptr == NULL && num_group == 0 && !setup_flag) goto control_error;
		} else {
			str_ptr = Get_Control_String (MASTER_CONTROL_FILE_x, i);
		}
		if (str_ptr == NULL) continue;

		Print (1);
		group.group_num = i;

		//---- open master control file ----

		group.master_file = new Db_File ();
		if (i == 0) {
			group.master_file->File_Type ("Master Control File");
		} else {
			str_fmt (buffer, sizeof (buffer), "Master Control File #%d", i);
			group.master_file->File_Type (buffer);
		}
		ptr = strchr (str_ptr, '/');
		if (ptr == NULL) {
			ptr = strchr (str_ptr, '\\');
		}
		if (ptr == NULL) {
			str_cpy (buffer, sizeof (buffer), Project_Filename (str_ptr));
		} else {
			str_cpy (buffer, sizeof (buffer), str_ptr);
		}
		if (!group.master_file->Open (buffer)) {
			File_Error (group.master_file->File_Type (), group.master_file->Filename ());
		}

		//---- get the directory text ----

		if (i == 0) {
			str_ptr = Get_Control_String (DIRECTORY);
		} else {
			str_ptr = Get_Control_String (DIRECTORY_x, i);
		}
		if (str_ptr != NULL) {
			str_cpy (group.directory, sizeof (group.directory), str_ptr);
		} else {
			if (dir0 [0] != '\0') {
				str_cpy (group.directory, sizeof (group.directory), dir0);
			} else {
				group.directory [0] = '\0';
			}
		}
		if (group.directory [0] != '\0') {
			Print (1, "Directory = %s", group.directory);
		}

		//---- get the description text ----

		if (i == 0) {
			str_ptr = Get_Control_String (DESCRIPTION);
		} else {
			str_ptr = Get_Control_String (DESCRIPTION_x, i);
		}
		if (str_ptr != NULL) {
			Print (1, "Description = %s", str_ptr);
			str_cpy (group.description, sizeof (group.description), str_ptr);
		} else {
			if (i == 0) {
				str_fmt (buffer, sizeof (buffer), "---- %s ----", (char *) Title ());
				str_cpy (group.description, sizeof (group.description), buffer);
			} else {
				group.description [0] = '\0';
			}
		}

		//---- get the programs ----

		if (i == 0) {
			str_ptr = Get_Control_String (PROGRAMS);
		} else {
			str_ptr = Get_Control_String (PROGRAMS_x, i);
		}
		if (str_ptr != NULL) {
			str_cpy (group.programs, sizeof (group.programs), str_ptr);
		} else {
			if (i == 0) {
				str_cpy (group.programs, sizeof (group.programs), "script");
			} else {
				str_fmt (group.programs, sizeof (group.programs), "script%d", i);
			}
		}
		Print (1, "Programs = %s", group.programs);

		//---- get the run name ----

		if (i == 0) {
			str_ptr = Get_Control_String (RUN_NAME);
		} else {
			str_ptr = Get_Control_String (RUN_NAME_x, i);
		}
		if (str_ptr != NULL) {
			str_cpy (group.run_name, sizeof (group.run_name), str_ptr);
		} else if (run0 [0] != '\0') {
			str_cpy (group.run_name, sizeof (group.run_name), run0);
		} else {
			group.run_name [0] = '\0';
		}

		//---- get the control filename ----

		if (i == 0) {
			str_ptr = Get_Control_String (CONTROL_NAME);
		} else {
			str_ptr = Get_Control_String (CONTROL_NAME_x, i);
		}
		if (group.run_name [0] != '\0') {
			group.control [0] = '.';
			group.control [1] = '\0';
		} else {
			group.control [0] = '\0';
		}
		if (file_prefix [0] != '\0') {
			str_cat (group.control, sizeof (group.control), file_prefix);
		}
		if (str_ptr != NULL) {
			str_cat (group.control, sizeof (group.control), str_ptr);
		} else {
			Get_Token (group.programs, buffer, sizeof (buffer));
			str_cat (group.control, sizeof (group.control), buffer);
		}
		len = (int) strlen (group.control);
		ptr = strchr (group.control + (len - 4), '.');
		if (ptr == NULL) {
			str_cat (group.control, sizeof (group.control), ".ctl");
		}
		Print (1, "Control Name = %s%s", group.run_name, group.control);

		if (group.run_name [0] != '\0') {
			Print (1, "Run Name = %s", group.run_name);
		}

		//---- get the previous run name ----

		if (i == 0) {
			str_ptr = Get_Control_String (PREVIOUS_RUN_NAME);
		} else {
			str_ptr = Get_Control_String (PREVIOUS_RUN_NAME_x, i);
		}
		if (str_ptr != NULL) {
			str_cpy (group.old_name, sizeof (group.old_name), str_ptr);
		} else {
			if (previous0 [0] != '\0') {
				str_cpy (group.old_name, sizeof (group.old_name), previous0);
			} else {
				group.old_name [0] = '\0';
			}
		}
		if (group.old_name [0] != '\0') {
			Print (1, "Previous Run Name = %s", group.old_name);
		}

		//---- get the parameters ----

		for (j=0; j < 4; j++) {
			if (i == 0) {
				str_ptr = Get_Control_String (param [j]);
			} else {
				str_ptr = Get_Control_String (paramx [j], i);
			}
			if (str_ptr != NULL) {
				str_cpy (group.parameter [j], sizeof (group.parameter [j]), str_ptr);
			} else {
				if (param0 [j][0] != '\0') {
					str_cpy (group.parameter [j], sizeof (group.parameter [j]), param0 [j]);
				} else {
					group.parameter [j][0] = '\0';
				}
			}
			if (group.parameter [j][0] != '\0') {
				if (j == 0) {
					Print (1, "Parameter = %s", group.parameter [j]);
				} else {
					Print (1, "Parameter%d = %s", j, group.parameter [j]);
				}
			}
		}

		//---- get the append keys ----

		for (j=0; j < 4; j++) {
			if (i == 0) {
				str_ptr = Get_Control_String (append [j]);
			} else {
				str_ptr = Get_Control_String (appendx [j], i);
			}
			if (str_ptr != NULL) {
				str_cpy (group.add_key [j], sizeof (group.add_key [j]), str_ptr);
			} else {
				if (append0 [j][0] != '\0') {
					str_cpy (group.add_key [j], sizeof (group.add_key [j]), append0 [j]);
				} else {
					group.add_key [j][0] = '\0';
				}
			}
			if (group.add_key [j][0] != '\0') {
				if (j == 0) {
					Print (1, "Append Key = %s", group.add_key [j]);
				} else {
					Print (1, "Append%d Key = %s", j, group.add_key [j]);
				}
			}
		}

		//---- get the comment flags ----

		for (j=0; j < 4; j++) {

			if (i == 0) {
				str_ptr = Get_Control_String (comment [j]);
			} else {
				str_ptr = Get_Control_String (commentx [j], i);
			}
			if (str_ptr != NULL) {
				if (i == 0) {
					group.comment_flag [j] = comment_flag0 [j];
				} else {
					group.comment_flag [j] = Get_Control_Flag (commentx [j], i);
				}
			} else {
				if (flag0 [j]) {
					group.comment_flag [j] = comment_flag0 [j];
				} else {
					group.comment_flag [j] = false;
				}
			}
			if (group.comment_flag [j]) {
				if (j == 0) {
					Print (1, "Comment Flag is TRUE");
				} else {
					Print (1, "Comment%d Flag is TRUE", j);
				}
			}
		}

		//---- add the files to the group ----

		if (!control_group.Add (&group)) goto mem_error;
	}

	//---- get the num splits ----

	str_ptr = Get_Control_String (NUM_SPLIT_FILES);

	if (str_ptr != NULL) {
		str_cpy (num_splits, sizeof (num_splits), str_ptr);

		Print (2, "Number of Split Files = %s", str_ptr);
	} else {
		num_splits [0] = '\0';
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for Control Files");
} 
