//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "IntControl.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void IntControl::Program_Control (void)
{
	int i;
	double dvalue;
	char *str_ptr;

	char *split_method [] = {
		"LANES", "CAPACITY", "VOLUME", "UPDATE",
		"LANE_SPLITS", "CAPACITY_SPLITS", "VOLUME_SPLITS", "UPDATE", NULL
	};
	int split_code [] = {
		LANE_SPLITS, CAPACITY_SPLITS, VOLUME_SPLITS, UPDATE_SPLITS, 
		LANE_SPLITS, CAPACITY_SPLITS, VOLUME_SPLITS, UPDATE_SPLITS
	};

	//---- split method (LANES/CAPACITY/VOLUME/UPDATE) ----

	delete_flag = (Get_Control_String (DELETE_NODE_CONTROL_FILE) != NULL);

	str_ptr = Get_Control_String (SIGNAL_SPLIT_METHOD);

	if (str_ptr != NULL) {
		for (i=0; split_method [i] != NULL; i++) {
			if (str_cmp (str_ptr, split_method [i]) == 0) break;
		}
		if (split_method [i] == NULL) {
			Error ("Unrecognized Split Method %s", str_ptr);
		}
		method = split_code [i];

		if (method == UPDATE_SPLITS && !delete_flag) {
			Network_File_False (DETECTOR);
			Network_File_False (SIGNAL_COORDINATOR);
			Network_File_False (NEW_SIGNALIZED_NODE);
			Network_File_False (NEW_PHASING_PLAN);
			Network_File_False (NEW_DETECTOR);
			Network_File_False (NEW_SIGNAL_COORDINATOR);
		}
	}

	//---- create the network files ----

	Network_Service::Program_Control ();

	sign_flag = Network_File_Flag (NEW_UNSIGNALIZED_NODE);	
	signal_flag = Network_File_Flag (NEW_SIGNALIZED_NODE) || Network_File_Flag (NEW_TIMING_PLAN);

	if (!sign_flag && !signal_flag) {
		Error ("No Sign or Signal Output is Requested");
	}

	//---- open the delete node control file ----

	str_ptr = Get_Control_String (DELETE_NODE_CONTROL_FILE);

	if (str_ptr != NULL) {
		Print (1);
		delete_file.File_Type ("Delete Node Control File");

		if (!delete_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening Delete Node Control File", str_ptr);
		}
		delete_flag = true;
	}

	//---- process sign keys ----

	if (sign_flag) {

		//---- open the input sign file ----

		str_ptr = Get_Control_String (INPUT_SIGN_FILE);

		if (str_ptr != NULL) {
			Print (1);
			sign_file.File_Type ("Input Sign File");

			if (!sign_file.Open (Project_Filename (str_ptr))) {
				File_Error ("Opening Input Sign File", str_ptr);
			}
			input_sign_flag = true;
		}

		//---- get the no control flag ----

		str_ptr = Get_Control_String (ADD_NO_CONTROL_RECORDS);

		if (str_ptr != NULL) {
			no_control_flag = Get_Control_Flag (ADD_NO_CONTROL_RECORDS);
		}
		if (no_control_flag) {
			Print (2, "No Control Records will be Added to the Unsignalized Node file");
		} else {
			Print (2, "No Control Records will not be Generated");
		}

		//---- sign warning messages ----

		str_ptr = Get_Control_String (PRINT_SIGN_WARNINGS);

		if (str_ptr != NULL) {
			warning_flag = Get_Control_Flag (PRINT_SIGN_WARNINGS);

			if (warning_flag) {
				Print (2, "Sign Warning Messages will be Printed");
			} else {
				Print (2, "Sign Warning Messages will not be Printed");
			}
		}

		if (!input_sign_flag && !Network_File_Flag (UNSIGNALIZED_NODE) && !delete_flag) {
			Write (1);
			Warning ("Input Signs were Not Provided");
			if (no_control_flag) {
				Write (1, "\t\tAll Unsignalized Nodes will have No Control");
			} else {
				Write (1, "\t\tThe Output Unsignalized Node File will be Empty");
				sign_flag = false;
			}
		}

		//---- set the field names ----

		if (Network_File_Flag (UNSIGNALIZED_NODE)) {
			sign_db.Replicate_Fields (Network_Db_Base (UNSIGNALIZED_NODE));
			sign_db.Add_Field ("SIGN_INDEX", INTEGER, 10);
			sign_index = sign_db.Field_Number ("SIGN_INDEX");

			Network_Copy_Header (UNSIGNALIZED_NODE, NEW_UNSIGNALIZED_NODE);
		}
	}

	//---- process signal keys ----

	if (!signal_flag) return;

	//---- open the input signal file ----

	str_ptr = Get_Control_String (INPUT_SIGNAL_FILE);

	if (str_ptr != NULL) {
		Print (1);
		signal_file.File_Type ("Input Signal File");

		if (!signal_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening Input Signal File", str_ptr);
		}
		input_signal_flag = true;
	}

	//---- check the signal processing keys ----

	if (!input_signal_flag && !Network_File_Flag (SIGNALIZED_NODE)) {
		Write (1);
		Warning ("Input Signals were Not Provided");
		Write (1, "\t\tThe Output Signal Files will be Empty");
		signal_flag = false;
		return;
	}
	if (method == UPDATE_SPLITS) {
		update_flag = true;

		if (!Network_File_Flag (SIGNALIZED_NODE) || !Network_File_Flag (TIMING_PLAN) ||
			!Network_File_Flag (PHASING_PLAN)) {
			Error ("Signal, Timing, and Phasing files are Required for Timing Updates");
		}
		if (!Network_File_Flag (NEW_TIMING_PLAN)) {
			Error ("A New Timing Plan is Required for Timing Updates");
		}
	} else {
		if (!Network_File_Flag (NEW_SIGNALIZED_NODE)) {
			Error ("A New Signalized Node File is Required to Create Signals");
		}
		if (input_signal_flag) {
			if (!Network_File_Flag (SIGNALIZED_NODE)) {
				if (delete_flag) {
					Error ("Signalized Node File is Required for Signal Deletes");
				}
				if (Network_File_Flag (TIMING_PLAN) || Network_File_Flag (PHASING_PLAN) || 
					Network_File_Flag (DETECTOR)) {
					Error ("Signalized Node File is Required for Signal Edits");
				}
			} else if (!Network_File_Flag (TIMING_PLAN) || !Network_File_Flag (PHASING_PLAN)) {
				Error ("Timing and Phasing files are Required for Signal Changes");
			}
		} else if (Network_File_Flag (TIMING_PLAN) || Network_File_Flag (PHASING_PLAN)) {
			if (!delete_flag) {
				Error ("Conflict between Timing/Phasing files and No Input Signals");
			}
		} else if (delete_flag) {
			Error ("Timing and Phasing files are Required for Signal Deletes");
		} else {
			regen_flag = true;
		}
		if (!Network_File_Flag (NEW_TIMING_PLAN) ||	!Network_File_Flag (NEW_PHASING_PLAN)) {
			Error ("New Timing and Phasing Plan files are Required for Signal Processing");
		}
		if (!Network_File_Flag (NEW_DETECTOR)) {
			Write (1);
			Warning ("If Actuated Signals are generated, a New Detector file is needed");
		}
	}

	//---- set the field names ----

	if (Network_File_Flag (TIMING_PLAN)) {
		timing_db.Replicate_Fields (Network_Db_Base (TIMING_PLAN));

		Network_Copy_Header (TIMING_PLAN, NEW_TIMING_PLAN);
	}
	if (delete_flag || !update_flag) {
		if (Network_File_Flag (SIGNALIZED_NODE)) {
			signal_db.Replicate_Fields (Network_Db_Base (SIGNALIZED_NODE));
			signal_db.Add_Field ("SIGNAL_INDEX", INTEGER, 10);
			signal_index = signal_db.Field_Number ("SIGNAL_INDEX");

			Network_Copy_Header (SIGNALIZED_NODE, NEW_SIGNALIZED_NODE);
		}
		if (Network_File_Flag (PHASING_PLAN)) {
			phasing_db.Replicate_Fields (Network_Db_Base (PHASING_PLAN));
			phasing_db.Add_Field ("PHASING_INDEX", INTEGER, 10);
			phasing_index = phasing_db.Field_Number ("PHASING_INDEX");

			Network_Copy_Header (PHASING_PLAN, NEW_PHASING_PLAN);
		}
		if (Network_File_Flag (DETECTOR) && Network_File_Flag (NEW_DETECTOR)) {
			detector_flag = true;
			detector_db.Replicate_Fields (Network_Db_Base (DETECTOR));

			Network_Copy_Header (DETECTOR, NEW_DETECTOR);
		}
		if (Network_File_Flag (SIGNAL_COORDINATOR) && Network_File_Flag (NEW_SIGNAL_COORDINATOR)) {
			coord_flag = true;
			coord_db.Replicate_Fields (Network_Db_Base (SIGNAL_COORDINATOR));

			Network_Copy_Header (SIGNAL_COORDINATOR, NEW_SIGNAL_COORDINATOR);
		}
	}
	if (Network_File_Flag (NEW_SIGNALIZED_NODE)) {
		if (Network_Db_Base (NEW_SIGNALIZED_NODE)->Dbase_Format () == VERSION3) {
			time_range.Format ("TIME_CODE");
		}
	}

	//---- sign merge messages ----

	str_ptr = Get_Control_String (PRINT_MERGE_WARNINGS);

	if (str_ptr != NULL) {
		merge_flag = Get_Control_Flag (PRINT_MERGE_WARNINGS);

		if (merge_flag) {
			Print (2, "Merge Warning Messages will be Printed");
		} else {
			Print (2, "Merge Warning Messages will not be Printed");
		}
	}

	//---- get the cycle length ----
	
	str_ptr = Get_Control_String (SIGNAL_CYCLE_LENGTH);

	Print (2, "Signal Cycle Length =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &cycle_len);
		}
		if (cycle_len < 30 || cycle_len > 240) {
			Error ("Cycle Length %d is Out of Range (30..240)", cycle_len);
		}
		Print (0, " %d ", cycle_len);

		if (!cycle_len_list.Add (cycle_len)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "seconds");

	//---- get the minimum phase time ----

	str_ptr = Get_Control_String (MINIMUM_PHASE_TIME);

	Print (1, "Minimum Phase Time =");
	i = 1;

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &min_phase);
		}
		cycle_len = cycle_len_list.Best (i++);

		if (min_phase < 1 || min_phase > cycle_len / 2) {
			Error ("Minimum Phase Time %d is Out of Range (1..%d)", min_phase, cycle_len / 2);
		}
		Print (0, " %d ", min_phase);

		if (!min_phase_list.Add (min_phase)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "seconds");

	//---- get the yellow phase time ----

	str_ptr = Get_Control_String (YELLOW_PHASE_TIME);
	
	Print (1, "Yellow Phase Time =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &yellow);
		}
		if (yellow < 1 || yellow > 5) {
			Error ("Yellow Phase Time %d is Out of Range (1..5)", yellow);
		}
		Print (0, " %d ", yellow);

		if (!yellow_list.Add (yellow)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "seconds");

	//---- get the red clear phase time ----

	str_ptr = Get_Control_String (RED_CLEAR_PHASE_TIME);

	Print (1, "Red Clear Phase Time =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Integer (str_ptr, &red_clear);
		}
		if (red_clear < 0 || red_clear > 5) {
			Error ("Red Clear Phase Time %d is Out of Range (0..5)", red_clear);
		}
		Print (0, " %d ", red_clear);

		if (!red_clear_list.Add (red_clear)) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "seconds");

	//---- get the pocket factor ----

	str_ptr = Get_Control_String (POCKET_LANE_FACTOR);
	
	Print (1, "Pocket Lane Weighting Factor =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &pocket_factor);
		}
		if (pocket_factor < 0.0 || pocket_factor > 1.0) {
			Error ("Pocket Lane Weighting Factor %.2lf is Out of Range (0..1)", pocket_factor);
		}
		Print (0, " %.2lf ", pocket_factor);

		if (!pocket_factor_list.Add ((int) (pocket_factor * 1000.0 + 0.5))) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "of a Thru Lane");

	//---- get the general factor ----

	str_ptr = Get_Control_String (GENERAL_GREEN_FACTOR);

	Print (1, "General Green Weighting Factor =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &general_factor);
		}
		if (general_factor < 0.5 || general_factor > 1.0) {
			Error ("General Green Weighting Factor %.2lf is Out of Range (0.5..1.0)", general_factor);
		}
		Print (0, " %.2lf ", general_factor);

		if (!general_factor_list.Add ((int) (general_factor * 1000.0 + 0.5))) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "of the Combined Approach");

	//---- get the extend green factor ----

	str_ptr = Get_Control_String (EXTENDED_GREEN_FACTOR);
	
	Print (1, "Extended Green Factor =");

	do {
		if (str_ptr != NULL) {
			str_ptr = Get_Double (str_ptr, &extend_factor);
		}
		if (extend_factor < 0.0 || extend_factor > 1.0) {
			Error ("Extended Green Factor %.2lf is Out of Range (0..1)", extend_factor);
		}
		Print (0, " %.2lf ", extend_factor);

		if (!extend_factor_list.Add ((int) (extend_factor * 1000.0 + 0.5))) goto mem_error;
	} while (str_ptr != NULL);

	Print (0, "of Minimum Green Time");
	Print (1);

	//---- split method (LANES/CAPACITY/VOLUME/UPDATE) ----

	Print (1, "Signal Split Weighting Method = %s", split_method [method]);

	//---- get min/max values ----

	if (method == CAPACITY_SPLITS) {

		str_ptr = Get_Control_String (MINIMUM_LANE_CAPACITY);

		Print (1, "\tMinimum Lane Capacity =");

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &min_cap);
			}
			Print (0, " %d ", min_cap);

			if (!min_cap_list.Add (min_cap)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "vphpl");

		str_ptr = Get_Control_String (MAXIMUM_LANE_CAPACITY);
		
		Print (1, "\tMaximum Lane Capacity =");
		i = 1;

		do {
			if (str_ptr != NULL) {
				str_ptr = Get_Integer (str_ptr, &max_cap);
			}
			min_cap = min_cap_list.Best (i++);

			if (min_cap > max_cap || min_cap < 0 || max_cap < 0) {
				Error ("Minimum/Maximum Capacity is Illegal (%d, %d)", min_cap, max_cap);
			} else if (max_cap > 2000) {
				Error ("Maximum Capacity is Out of Range (%d > 2000 vphpl)", max_cap);
			}
			Print (0, " %d ", max_cap);

			if (!max_cap_list.Add (max_cap)) goto mem_error;
		} while (str_ptr != NULL);

		Print (0, "vphpl");
	}

	//---- get the detector length ----

	str_ptr = Get_Control_String (SIGNAL_DETECTOR_LENGTH);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &dvalue);

		if (dvalue < 5.0 || dvalue > 50.0) {
			Error ("Signal Detector Length %.2lf is Out of Range (5..50)", dvalue);
		}
		detector_length = Round (dvalue);
	}
	Print (2, "Signal Detector Length = %.2lf meters", UnRound (detector_length));

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_range.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- time period breaks ----

	str_ptr = Get_Control_String (TIME_PERIOD_BREAKS);

	if (str_ptr != NULL) {
		if (!time_range.Add_Breaks (str_ptr)) {
			Error ("Adding Time Period Breaks");
		}
		Print (2, "Time Period Breaks = %s", str_ptr);
		time_flag = true;
	}

	//---- time period range ----

	str_ptr = Get_Control_String (TIME_PERIOD_RANGE);

	if (str_ptr != NULL) {
		if (time_flag) {
			Warning ("Use Time Period Breaks or Range, but not both");
		} else {
			if (update_flag) {
				Error ("Time Period Range can only be used for Updates");
			}
			if (!time_range.Add_Ranges (str_ptr)) {
				Error ("Adding Time Period Range");
			}
			Print (2, "Time Period Range = %s", str_ptr);
			time_flag = range_flag = true;
		}
	}

	//---- turn movement file ----

	str_ptr = Get_Control_String (TURN_MOVEMENT_FILE);

	if (str_ptr != NULL) {
		Print (1);
		turn_file.Open (Project_Filename (str_ptr));

		turn_flag = true;
	}

	//---- link delay file ----

	str_ptr = Get_Control_String (LINK_DELAY_FILE);

	if (str_ptr != NULL) {
		if (turn_flag) {
			Error ("Use Link Delay or Turn Movements, but not both");
		}
		Print (1);
		delay_file.Open (Project_Filename (str_ptr));

		if (!delay_file.Nested ()) {
			Error ("The Link Delay File does not include Turning Movements");
		}
		delay_flag = true;
	}
	if (turn_flag || delay_flag) {
		if (method != VOLUME_SPLITS && !update_flag) {
			Warning ("Link Delay or Turn Movements are used by Volume or Update Methods");
		}
	} else if (method == VOLUME_SPLITS || update_flag) {
		Error ("Volume or Update Methods require Link Delay or Turn Movements");
	}

	update_report = Report_Flag (TIMING_UPDATE);
	return;

mem_error:
	Error ("Insufficient Memory for Period Lists");
} 
