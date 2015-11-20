//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "FileFormat.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void FileFormat::Program_Control (void)
{
	int i, header_lines, default_lines, default_format;
	char *str_ptr, file_label [FIELD_BUFFER], *format_ptr;
	char dir [FILE_NAME_SIZE];

	//---- retrieve the default file format ----

	Db_Code format_code;

	default_format = format_code.Default_Format ();
	Print (1);

	//---- get the input directory ----

	str_ptr = Get_Control_String (INPUT_DIRECTORY);

	if (str_ptr != NULL) {
		str_cpy (dir, sizeof (dir), str_ptr);

		Print_Filename (1, "Input Directory", dir);

		Fix_Directory (dir);
	} else {
		str_cpy (dir, sizeof (dir), Project_Directory ());
	}

	//---- get the input format ----

	format_ptr = Get_Control_String (INPUT_FORMAT);

	if (format_ptr != NULL) {
		Print (1, "Input Format = %s", format_ptr);
		format_code.Default_Format (format_code.Format_Code (format_ptr));
	}

	//---- get the input header lines ----

	str_ptr = Get_Control_String (INPUT_HEADER_LINES);

	if (str_ptr != NULL) {
		default_lines = atoi (str_ptr);
		Print (1, "Input Header Lines = %d", default_lines);
	} else {
		default_lines = 1;
	}

	//---- get the number of files ----

	nfiles = Highest_Control_Group (INPUT_FILE_x, 0);

	if (nfiles == 0) {
		str_ptr = Get_Control_String (INPUT_FILE);
		if (str_ptr == NULL) goto control_error;
		nfiles = 1;
	} else {
		nfiles++;
	}

	//---- allocate file points ----

	input_file = new Db_Header * [nfiles];
	output_file = new Db_Header * [nfiles];
	nest_field = new int [nfiles];

	if (input_file == NULL || output_file == NULL || nest_field == NULL) goto mem_error;

	for (i=0; i < nfiles; i++) {
		input_file [i] = NULL;
		output_file [i] = NULL;
	}

	//---- open each input file ----

	for (i=0; i < nfiles; i++) {
		if (i) {
			str_ptr = Get_Control_String (INPUT_HEADER_LINES_x, i);

			if (str_ptr != NULL) {
				header_lines = atoi (str_ptr);
			} else {
				header_lines = default_lines;
			}
			str_ptr = Get_Control_String (INPUT_FILE_x, i);
			str_fmt (file_label, sizeof (file_label), "Input File #%d", i);

			format_ptr = Get_Control_String (INPUT_FORMAT_x, i);
		} else {
			str_ptr = Get_Control_String (INPUT_FILE);
			str_cpy (file_label, sizeof (file_label), "Input File");

			header_lines = default_lines;
			format_ptr = NULL;
		}
		if (str_ptr == NULL) continue;

		input_file [i] = new Db_Header ();

		if (input_file [i] == NULL) goto mem_error;

		input_file [i]->File_Type (file_label);

		if (format_ptr != NULL) {
			input_file [i]->Dbase_Format (format_ptr);
		}
		input_file [i]->Header_Lines (header_lines);

		input_file [i]->Open (Filename (dir, str_ptr));

		if (header_lines != default_lines) {
			Print (1, "Number of Header Lines = %d", header_lines);
		}

		//----- get the nested count field ----

		if (input_file [i]->Nested ()) {
			if (i) {
				str_ptr = Get_Control_String (NESTED_COUNT_FIELD_x, i);
			} else {
				str_ptr = Get_Control_String (NESTED_COUNT_FIELD);
			}
			if (str_ptr == NULL) goto control_error;

			nest_field [i] = input_file [i]->Required_Field (str_ptr);

			Print (1, "Nested Count Field = %s, Number = %d", str_ptr, nest_field [i]);
		}
	}

	//---- reset the default file format ----

	format_code.Default_Format (default_format);

	//---- get the output format ----

	format_ptr = Get_Control_String (OUTPUT_FORMAT);

	if (format_ptr != NULL) {
		Print (2, "Output Format = %s", format_ptr);
		format_code.Default_Format (format_code.Format_Code (format_ptr));
	} else {
		Print (1);
	}

	//---- get the output directory ----

	str_ptr = Get_Control_String (OUTPUT_DIRECTORY);

	if (str_ptr != NULL) {
		str_cpy (dir, sizeof (dir), str_ptr);

		Print_Filename (1, "Output Directory", dir);

		Fix_Directory (dir);
	} else {
		str_cpy (dir, sizeof (dir), Project_Directory ());
	}

	//---- open each output file ----

	for (i=0; i < nfiles; i++) {
		if (input_file [i] == NULL) continue;

		if (i) {
			str_ptr = Get_Control_String (OUTPUT_FILE_x, i);
			str_fmt (file_label, sizeof (file_label), "Output File #%d", i);

			format_ptr = Get_Control_String (OUTPUT_FORMAT_x, i);
		} else {
			str_ptr = Get_Control_String (OUTPUT_FILE);
			str_cpy (file_label, sizeof (file_label), "Output File");

			format_ptr = NULL;
		}
		if (str_ptr == NULL) goto control_error;

		output_file [i] = new Db_Header ();

		if (output_file [i] == NULL) goto mem_error;

		output_file [i]->File_Type (file_label);

		if (format_ptr != NULL) {
			output_file [i]->Dbase_Format (format_ptr);
		}
		output_file [i]->Header_Lines (input_file [i]->Header_Lines ());
		output_file [i]->Nested (input_file [i]->Nested ());

		output_file [i]->Create (Filename (dir, str_ptr));
		output_file [i]->Replicate_Fields (input_file [i]);
		output_file [i]->Write_Header (input_file [i]->Header_Record ());

		if (output_file [i]->Header_Lines () != default_lines) {
			Print (1, "Number of Header Lines = %d", output_file [i]->Header_Lines ());
		}
	}

	//---- reset the default file format ----

	format_code.Default_Format (default_format);

	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Allocating Space for %d Files", nfiles);
}
