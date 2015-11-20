//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "HHList.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void HHList::Program_Control (void)
{
	int i;
	char *str_ptr, *format_ptr, buffer [STRING_BUFFER];

	Print (1);

	//---- open the input data file ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		list_file.File_Type ("Household List");
		list_file.Open (Project_Filename (str_ptr));
		list_flag = true;
	} else {
		str_ptr = Get_Control_String (HOUSEHOLD_FILE);

		if (str_ptr != NULL) {
			input_file.File_Type ("Household File");

			format_ptr = Get_Control_String (HOUSEHOLD_FORMAT);
		} else {
			str_ptr = Get_Control_String (TRIP_FILE);

			if (str_ptr != NULL) {
				input_file.File_Type ("Trip File");

				format_ptr = Get_Control_String (TRIP_FORMAT);

				two_flag = (Get_Control_String (ACTIVITY_FILE) != NULL);
			} else {
				str_ptr = Get_Control_String (ACTIVITY_FILE);

				if (str_ptr != NULL) {
					input_file.File_Type ("Activity File");

					format_ptr = Get_Control_String (ACTIVITY_FORMAT);
				} else {
					Error ("A Household List, Household File, Trip File or Activity File is required");
				}
			}
		}
		if (format_ptr != NULL) {
			input_file.File_Format (format_ptr);
		}
		input_file.Open (Project_Filename (str_ptr));
	}

	//---- read two files ----

	if (two_flag) {
		str_ptr = Get_Control_String (ACTIVITY_FILE);

		input2_file.File_Type ("Activity File");

		if (format_ptr != NULL) {
			input2_file.File_Format (format_ptr);
		}
		input2_file.Open (Project_Filename (str_ptr));
	}

	//---- read the number of split files ----

	str_ptr = Get_Control_String (NUM_SPLIT_FILES);

	if (str_ptr == NULL) goto control_error;

	Get_Integer (str_ptr, &num_files);

	if (num_files < 1 || num_files > 200) {
		Error ("Number of Split Files %d is Out of Range (1..200)", num_files);
	}

	//---- open the output files ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr == NULL) goto control_error;

	output_file = new Db_File * [num_files];

	if (output_file == NULL) {
		Error ("Insufficient Memory for New Household Lists");
	}
	Print (1);

	for (i=0; i < num_files; i++) {
		str_fmt (buffer, sizeof (buffer), "%s.t%c%c", str_ptr, ('A' + (i / 26)), ('A' + (i % 26)));

		output_file [i] = new Db_File (Db_File::CREATE);
		output_file [i]->File_Type ("New Household List");

		if (!output_file [i]->Open (Project_Filename (buffer))) {
			File_Error ("Creating Household List", output_file [i]->Filename ());
		}
	}

	Print (2, "Number of Split Files = %d", num_files);

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (1, "Random Number Seed = %d", random.Seed ());

	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
