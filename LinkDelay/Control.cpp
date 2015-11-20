//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "LinkDelay.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkDelay::Program_Control (void)
{
	char *str_ptr, *format_ptr;

	Combine_Method method_code [] = {
		REPLACE_LINKS, SIMPLE_AVERAGE, WEIGHTED_AVERAGE
	};
	char *method_text [] = {
		"REPLACE_LINKS", "SIMPLE_AVERAGE", "WEIGHTED_AVERAGE", NULL
	};

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- add data layers for additional field processing ----

	ttime_data.Layers (12);

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_step.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (1, "Time of Day Format = %s", str_ptr);

	//---- open the previous link delay ----
	
	str_ptr = Get_Control_String (PREVIOUS_LINK_DELAY_FILE);

	if (str_ptr != NULL) {
		previous_delay.File_Type ("Previous Link Delay File");
		Print (1);

		format_ptr = Get_Control_String (PREVIOUS_LINK_DELAY_FORMAT);

		if (format_ptr != NULL) {
			previous_delay.Dbase_Format (format_ptr);
		}
		previous_delay.Open (Project_Filename (str_ptr));
		previous_flag = true;

		str_ptr = Get_Control_String (PREVIOUS_WEIGHTING_FACTOR);

		if (str_ptr != NULL) {
			factor = atof (str_ptr);

			if (factor != 0.0 && factor < 0.5) {
				Error ("Previous Weighting Factor %.2lf is Out of Range", factor);
			}
		} else {
			factor = previous_delay.Iteration ();
		}
		if (factor == 0.0) {
			Print (1, "Previous Weighting is based on Volume data");
		} else {
			Print (1, "Previous Weighting Factor = %.2lf", factor);
		}

		//---- open the previous link file ----

		str_ptr = Get_Control_String (PREVIOUS_LINK_FILE);

		if (str_ptr != NULL) {
			previous_link.File_Type ("Previous Link File");
			Print (1);
			previous_link.Open (Project_Filename (str_ptr));
			link_flag = true;
			AB_Key_Flag (true);
		}
	}
	
	//---- set the output link delay ----

	Delay_File *output_delay = (Delay_File *) Demand_Db_File (NEW_LINK_DELAY);

	output_delay->Increment (ttime_data.Period_Size ());

	if (previous_flag) {
		output_delay->Iteration (previous_delay.Iteration () + 1);
	}
	output_delay->Create_Header ();

	if (output_delay->Dbase_Format () != VERSION3) {
		output_delay = (Delay_File *) Demand_Db_File (LINK_DELAY);

		if (output_delay->Dbase_Format () != VERSION3 && 
			!Network_File_Flag (LANE_CONNECTIVITY)) {
			Warning ("Lane Connectivity is Required to Output Movement Data");
			Show_Message (1);
		}
	}

	//---- processing method ----

	if (previous_flag) {
		int i;
		str_ptr = Get_Control_String (PROCESSING_METHOD);

		if (str_ptr == NULL) goto control_error;

		for (i=0; method_text [i] != NULL; i++) {
			if (str_cmp (str_ptr, method_text [i]) == 0) {
				method = method_code [i];
				break;
			}
		}
		if (method_text [i] == NULL) {
			Error ("Processing Method %s was Unrecognized", str_ptr);
		}
		Print (2, "Processing Method = %s", method_text [i]);
	}

	//---- read the sort flag ----

	str_ptr = Get_Control_String (TIME_PERIOD_SORT);

	if (str_ptr != NULL) {
		sort_flag = Get_Control_Flag (TIME_PERIOD_SORT);

		Print (2, "Time Period Sort = %s", str_ptr);
	}

	//---- read the number smooth records ----

	str_ptr = Get_Control_String (SMOOTH_GROUP_SIZE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &naverage);
	}
	if (naverage == 0) {
		Print (2, "Delay Smoothing is Disabled");
		return;
	}
	Print (2, "Smooth Group Size = %d", naverage);

	//---- read the percent distributed forward ----

	str_ptr = Get_Control_String (PERCENT_MOVED_FORWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &forward);
	}
	Print (1, "Percent Distributed Forward = %.1lf percent", forward);

	//---- read the percent distributed backwarde ----
	
	str_ptr = Get_Control_String (PERCENT_MOVED_BACKWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &backward);
	}
	Print (1, "Percent Distributed Backward = %.1lf percent", backward);

	//---- number of iterations ----

	str_ptr = Get_Control_String (NUMBER_OF_ITERATIONS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &niter);
	}
	Print (1, "Number of Iterations = %d", niter);

	//---- read the circular smoothing flag ----
	
	str_ptr = Get_Control_String (CIRCULAR_GROUP_FLAG);

	if (str_ptr != NULL) {
		loop_flag = Get_Control_Flag (CIRCULAR_GROUP_FLAG);
	}
	Print (1, "Circular Group Flag = %s", ((loop_flag) ? "TRUE" : "FALSE"));

	//---- set the smoothing parameters ----

	if (!smooth_data.Setup (ttime_data.Periods (), niter, loop_flag, naverage, forward, backward)) {
		Error ("Smooth Parameters are Illogical");
	}
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
