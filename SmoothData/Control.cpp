//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "SmoothData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SmoothData::Program_Control (void)
{
	int i, lvalue, nfile, nheader, nfield, increment, max_field;
	int naverage, default_average, niter, default_iter, max_size, max_iter;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];
	char input_data [FIELD_BUFFER], output_data [FIELD_BUFFER];
	char input_format [FIELD_BUFFER], output_format [FIELD_BUFFER];
	char initial_file [STRING_BUFFER], initial_format [FIELD_BUFFER];
	double dvalue, forward, backward, factor;
	bool loop_flag, initial_flag, replicate_flag;

	File_Group *group_ptr;

	nfile = 0;
	nheader = 1;
	nfield = 0;
	max_field = 100;
	increment = 0;
	default_average = 3;
	max_size = 9;
	forward = 20.0;
	backward = 20.0;
	default_iter = 0;
	max_iter = 25;
	loop_flag = true;
	initial_flag = false;
	replicate_flag = false;
	input_data [0] = output_data [0] = '\0';

	str_cpy (input_format, sizeof (input_format), "HOURS");
	str_cpy (output_format, sizeof (output_format), "HOURS");

	//---- number of input files ----

	nfile = Highest_Control_Group (INPUT_DATA_FILE_x, 0);

	if (nfile == 0) {
		Error ("No Input Data File Keys");
	}
	file_group.Max_Records (nfile);

	//---- get the default file format ----
	
	str_ptr = Get_Control_String (INPUT_DATA_FORMAT);

	if (str_ptr != NULL) {
		str_cpy (input_data, sizeof (input_data), str_ptr);
	}
	str_ptr = Get_Control_String (OUTPUT_DATA_FORMAT);

	if (str_ptr != NULL) {
		str_cpy (output_data, sizeof (output_data), str_ptr);
	}

	//---- get the default distribution filename ----

	str_ptr = Get_Control_String (DISTRIBUTION_FILE);

	if (str_ptr != NULL) {
		str_cpy (initial_file, sizeof (initial_file), str_ptr);
		initial_flag = true;

		str_ptr = Get_Control_String (DISTRIBUTION_FORMAT);

		if (str_ptr != NULL) {
			str_cpy (initial_format, sizeof (initial_format), str_ptr);
		} else {
			initial_format [0] = '\0';
		}
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (INPUT_TIME_FORMAT);

	if (str_ptr != NULL) {
		str_cpy (input_format, sizeof (input_format), str_ptr);
	}
	str_ptr = Get_Control_String (OUTPUT_TIME_FORMAT);

	if (str_ptr != NULL) {
		str_cpy (output_format, sizeof (output_format), str_ptr);
	}

	//---- read the field number ----	

	str_ptr = Get_Control_String (SMOOTH_FIELD_NUMBER);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);
		if (lvalue < 1 || lvalue > max_field) goto field_error;
		nfield = lvalue;
	}

	//---- time increment ----

	str_ptr = Get_Control_String (SMOOTH_TIME_INCREMENT);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);
		if (lvalue < 0 || lvalue > 3600) goto increment_error;
		increment = lvalue;
	}
	if (increment > 0) {
		max_size = 2 * 3600 / increment + 1;
		max_iter = (max_size + 1) / 2;
		if (max_iter > 100) max_iter = 100;
		max_iter *= max_iter;
	} else {
		max_size = 9;
		max_iter = 25;
	}

	//---- read the number smooth records ----

	str_ptr = Get_Control_String (SMOOTH_GROUP_SIZE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);
		if (lvalue < 3 || lvalue > max_size) goto size_error;
		if ((lvalue % 2) != 1) goto odd_error;
		default_average = lvalue;
	}

	//---- read the percent distributed forward ----

	factor = 100.0 - 50.0 / default_average;

	str_ptr = Get_Control_String (PERCENT_MOVED_FORWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);
		if (dvalue < 0.0 || dvalue > factor) goto forward_error;
		forward = dvalue;
	}

	//---- read the percent distributed backwarde ----
	
	str_ptr = Get_Control_String (PERCENT_MOVED_BACKWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);
		if (dvalue < 0.0 || dvalue > factor) goto backward_error;
		backward = dvalue;
	}

	factor = forward + backward;
	dvalue = 100.0 - 100.0 / default_average;
	if (factor < 5.0 || factor > dvalue) goto combined_error;

	//---- number of iterations ----

	str_ptr = Get_Control_String (NUMBER_OF_ITERATIONS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &lvalue);
		if (lvalue < 1 || lvalue > max_iter) goto iteration_error;
		default_iter = lvalue;
	}

	//---- read the circular smoothing flag ----
	
	str_ptr = Get_Control_String (CIRCULAR_GROUP_FLAG);

	if (str_ptr != NULL) {
		loop_flag = Get_Control_Flag (CIRCULAR_GROUP_FLAG);
	}

	//---- read the replicate fields flag ----
	
	str_ptr = Get_Control_String (REPLICATE_FIELDS_FLAG);

	if (str_ptr != NULL) {
		replicate_flag = Get_Control_Flag (REPLICATE_FIELDS_FLAG);
	}

	//---- open each file ----

	for (i=1; i <= nfile; i++) {

		//---- get the next filename ---
		
		str_ptr = Get_Control_String (INPUT_DATA_FILE_x, i);

		if (str_ptr == NULL) continue;

		group_ptr = file_group.New_Record (true);

		group_ptr->Group (i);

		//---- create the input file ----

		Print (1);
		group_ptr->Input_File (new Diurnal_File ());

		str_fmt (buffer, sizeof (buffer), "Input Data File #%d", i);

		group_ptr->Input_File ()->File_Type (buffer);

		format_ptr = Get_Control_String (INPUT_DATA_FORMAT_x, i);

		if (format_ptr != NULL) {
			group_ptr->Input_File ()->Dbase_Format (format_ptr);
		} else if (input_data [0] != '\0') {
			group_ptr->Input_File ()->Dbase_Format (input_data);
		}

		//---- open the file ----

		if (!group_ptr->Input_File ()->Open (Project_Filename (str_ptr))) {
			File_Error (buffer, group_ptr->Input_File ()->Filename ());
		}

		//---- open the output file ----

		str_ptr = Get_Control_String (OUTPUT_DATA_FILE_x, i);

		if (str_ptr == NULL) goto control_error;

		group_ptr->Output_File (new Diurnal_File (Db_Code::CREATE));
		
		str_fmt (buffer, sizeof (buffer), "Output Data File #%d", i);

		group_ptr->Output_File ()->File_Type (buffer);

		format_ptr = Get_Control_String (OUTPUT_DATA_FORMAT_x, i);

		if (format_ptr != NULL) {
			group_ptr->Output_File ()->Dbase_Format (format_ptr);
		} else if (output_data [0] != '\0') {
			group_ptr->Output_File ()->Dbase_Format (output_data);
		}
		if (!group_ptr->Output_File ()->Open (Project_Filename (str_ptr))) {
			File_Error (buffer, group_ptr->Output_File ()->Filename ());
		}

		//---- get the distribution file ----
		
		str_ptr = Get_Control_String (DISTRIBUTION_FILE_x, i);

		if (str_ptr == NULL) {
			if (initial_flag) {
				str_ptr = initial_file;
				if (initial_format [0] != '\0') {
					format_ptr = initial_format;
				} else {
					format_ptr = NULL;
				}
			}
		} else {
			format_ptr = Get_Control_String (DISTRIBUTION_FORMAT_x, i);
		}

		if (str_ptr != NULL) {
			group_ptr->Initial_File (new Diurnal_File ());

			str_fmt (buffer, sizeof (buffer), "Distribution File #%d", i);

			group_ptr->Initial_File ()->File_Type (buffer);

			if (format_ptr != NULL) {
				group_ptr->Initial_File ()->Dbase_Format (format_ptr);
			}
			if (!group_ptr->Initial_File ()->Open (Project_Filename (str_ptr))) {
				File_Error (buffer, group_ptr->Initial_File ()->Filename ());
			}
		} else {
			group_ptr->Initial_File (NULL);
		}

		//---- get the time of day format ----
		
		str_ptr = Get_Control_String (INPUT_TIME_FORMAT_x, i);

		if (str_ptr == NULL) {
			str_ptr = input_format;
		}
		if (!group_ptr->Input_Format (str_ptr)) {
			Error ("Input Time Format %s was Unrecognized", str_ptr);
		}
		Print (1, "Input Time Format = %s", str_ptr);
		
		str_ptr = Get_Control_String (OUTPUT_TIME_FORMAT_x, i);

		if (str_ptr == NULL) {
			str_ptr = output_format;
		}
		if (!group_ptr->Output_Format (str_ptr)) {
			Error ("Output Time Format %s was Unrecognized", str_ptr);
		}
		Print (1, "Output Time Format = %s", str_ptr);

		//---- read the field number ----	

		max_field = group_ptr->Input_File ()->Num_Fields ();

		str_ptr = Get_Control_String (SMOOTH_FIELD_NUMBER_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);
			if (lvalue < 1 || lvalue > max_field) goto field_error;
		} else {
			lvalue = nfield;
		}
		group_ptr->Field_Number (lvalue);

		if (lvalue > 0) {
			Print (1, "Smooth Field Number = %d", lvalue);
			group_ptr->Input_File ()->Share_Field (lvalue);
		}

		//---- time increment ----

		str_ptr = Get_Control_String (SMOOTH_TIME_INCREMENT_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);
			if (lvalue < 0 || lvalue > 3600) goto increment_error;
		} else {
			lvalue = increment;
		}
		if (lvalue > 0) {
			Print (1, "Smooth Time Increment = %d seconds", lvalue);

			max_size = 2 * 3600 / lvalue + 1;
			max_iter = (max_size + 1) / 2;
			if (max_iter > 100) max_iter = 100;
			max_iter *= max_iter;

			naverage = default_average;
			niter = default_iter;

			if (niter == 0) {
				niter = max_iter / 2;
			}
		} else {
			Print (1, "Smooth Based on Input Time Periods");

			max_size = 9;
			max_iter = 25;
			naverage = default_average; 
			niter = default_iter;
		}
		group_ptr->Increment (lvalue);

		//---- read the number smooth records ----

		str_ptr = Get_Control_String (SMOOTH_GROUP_SIZE_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);
			if (lvalue < 3 || lvalue > max_size) goto size_error;
			if ((lvalue % 2) != 1) goto odd_error;
		} else {
			lvalue = naverage;
		}
		Print (1, "Smooth Group Size = %d", lvalue);
		group_ptr->Num_Average (lvalue);

		//---- read the percent distributed forward ----

		factor = 100.0 - 50.0 / lvalue;

		str_ptr = Get_Control_String (PERCENT_MOVED_FORWARD_x, i);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);
			if (dvalue < 0.0 || dvalue > factor) goto forward_error;
		} else {
			dvalue = forward;
		}
		Print (1, "Percent Distributed Forward = %.1lf percent", dvalue);
		group_ptr->Forward (dvalue);

		//---- read the percent distributed backwarde ----
		
		str_ptr = Get_Control_String (PERCENT_MOVED_BACKWARD_x, i);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);
			if (dvalue < 0.0 || dvalue > factor) goto backward_error;
		} else {
			dvalue = backward;
		}
		Print (1, "Percent Distributed Backward = %.1lf percent", dvalue);
		group_ptr->Backward (dvalue);

		factor = group_ptr->Forward () + group_ptr->Backward ();
		dvalue = 100.0 - 100.0 / group_ptr->Num_Average ();
		if (factor < 5.0 || factor > dvalue) goto combined_error;

		//---- number of iterations ----

		str_ptr = Get_Control_String (NUMBER_OF_ITERATIONS_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);
			if (lvalue < 1 || lvalue > max_iter) goto iteration_error;
		} else {
			lvalue = niter;
		}
		Print (1, "Number of Iterations = %d", lvalue);
		group_ptr->Num_Iteration (lvalue);

		//---- read the circular smoothing flag ----
		
		str_ptr = Get_Control_String (CIRCULAR_GROUP_FLAG_x, i);

		if (str_ptr != NULL) {
			group_ptr->Loop_Flag (Get_Control_Flag (CIRCULAR_GROUP_FLAG_x, i));
		} else {
			group_ptr->Loop_Flag (loop_flag);
		}
		Print (1, "Circular Group Flag = %s", (group_ptr->Loop_Flag () ? "TRUE" : "FALSE"));

		//---- read the replicate fields flag ----
		
		if (Get_Control_String (REPLICATE_FIELDS_FLAG_x, i) != NULL) {
			group_ptr->Replicate_Flag (Get_Control_Flag (REPLICATE_FIELDS_FLAG_x, i));
		} else {
			group_ptr->Replicate_Flag (replicate_flag);
		}
		Print (1, "Replicate Fields Flag = %s", (group_ptr->Replicate_Flag () ? "TRUE" : "FALSE"));

		if (group_ptr->Replicate_Flag ()) {
			int field, num;
			bool binary;
			Db_Field *fld;
			Diurnal_File *file;

			group_ptr->Output_File ()->Share_Field (0);
			group_ptr->Output_File ()->Clear_Fields ();

			//---- copy all fields ----

			file = group_ptr->Input_File ();
			binary = (file->Record_Format () == Db_Code::BINARY);

			num = file->Num_Fields ();

			for (field=1; field <= num; field++) {
				fld = file->Field (field);
				if (fld == NULL) continue;

				if (field == file->Start_Field () || field == file->End_Field ()) {
					group_ptr->Output_File ()->Add_Field (fld->Name (), Db_Code::STRING, 20);
				} else {
					group_ptr->Output_File ()->Add_Field (fld->Name (), fld->Type (), fld->Size (), 
															fld->Decimal (), -1, binary);
				}
			}
			group_ptr->Output_File ()->Write_Header ();
			group_ptr->Output_File ()->Share_Field (group_ptr->Input_File ()->Share_Field ());
		}

		//---- add the group record ----

		if (!file_group.Add ()) {
			Error ("Adding File Group");
		}
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

field_error:
	Error ("Field Number %d is Out of Range (1..%d)", lvalue, max_field);

increment_error:
	Error ("Smooth Time Increment %d is Out of Range (0..3600)", lvalue);

size_error:
	Error ("Smooth Group Size %d is Out of Range (1..%d)", lvalue, max_size);

odd_error:
	Error ("Smooth Group Size %d must be an Odd Number", lvalue);

forward_error:
	Error ("Forward Percentage %.1lf is Out of Range (0..%.1lf)", dvalue, factor);

backward_error:
	Error ("Backward Percentage %.1lf is Out of Range (0..%.1lf)", dvalue, factor);

combined_error:
	Error ("Combined Distribution Percentage %.1lf is Out of Range (5..%.0lf)", factor, dvalue);

iteration_error:
	Error ("Number of Iterations %d is Out of Range (1-%d)", lvalue, max_iter);
}
