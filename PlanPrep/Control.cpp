//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "PlanPrep.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanPrep::Program_Control (void)
{
    int i, dump;
	char *str_ptr;

	Sort_Type input_sort;

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- open the input plans ----

	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (INPUT_PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	input_plans.File_Type ("Input Plan File");	

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, input_plans.File_Type (), str_ptr);

	input_plans.Filename (str_ptr);

	str_ptr = Get_Control_String (INPUT_PLAN_FORMAT);

	if (str_ptr != NULL) {
		input_plans.File_Format (str_ptr);
		Print (1, "%s Format = %s", input_plans.File_Type (), str_ptr);
	}
	input_plans.Node_Based_Flag (type_flag);

	if (!input_plans.Open (0)) {
		File_Error ("Opening Input Plan File", input_plans.Filename ());
	}
	type_flag = input_plans.Node_Based_Flag ();
	parts_flag = (input_plans.Extend () > 0);

	//---- input plan sort ----

	str_ptr = Get_Control_String (INPUT_PLAN_SORT);

	if (str_ptr != NULL) {
		Print (1, "Input Plan Sort = %s", str_ptr);

		if (str_cmp (str_ptr, "TRAVELER") == 0) {
			input_sort = TRAVELER_SORT;
		} else if (str_cmp (str_ptr, "TIME") == 0) {
			input_sort = TIME_SORT;
		} else {
			Error ("Unrecognized Input Plan Sort = %s", str_ptr);
		}
		input_plans.Plan_Sort (input_sort);
		input_sort_flag = true;
	} else {
		input_sort = input_plans.Plan_Sort ();
	}

	//---- open the merge plans ----

	str_ptr = Get_Control_String (MERGE_PLAN_FILE);

	if (str_ptr != NULL) {
		merge_plans.File_Type ("Merge Plan File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, merge_plans.File_Type (), str_ptr);

		merge_plans.Filename (str_ptr);
		merge_plans.Extend (input_plans.Extend ());

		str_ptr = Get_Control_String (MERGE_PLAN_FORMAT);

		if (str_ptr != NULL) {
			merge_plans.File_Format (str_ptr);
			Print (1, "%s Format = %s", merge_plans.File_Type (), str_ptr);
		}
		merge_plans.Node_Based_Flag (type_flag);

		if (!merge_plans.Open (0)) {
			File_Error ("Opening Merge Plan File", merge_plans.Filename ());
		}
		merge_flag = true;
		if (merge_plans.Node_Based_Flag () != type_flag) {
			Error ("Incompatible Plan Formats");
		}
	}

	//---- open the subarea plans ----

	str_ptr = Get_Control_String (SUBAREA_PLAN_FILE);

	if (str_ptr != NULL) {
		if (merge_flag) {
			Error ("Merge and Subarea Plans are Incompatible");
		}
		subarea_plans.File_Type ("Subarea Plan File");

		str_ptr = Project_Filename (str_ptr);

		subarea_plans.Filename (str_ptr);

		if (subarea_plans.Extend () > 0) {
			if (parts_flag) {
				subpart_flag = true;
			} else {
				str_ptr = subarea_plans.Pathname ();
				str_ptr = Project_Filename (str_ptr, Extension ());
				subarea_plans.Filename (str_ptr);
				subarea_plans.Extend (0);
			}
		}
		Print_Filename (2, subarea_plans.File_Type (), str_ptr);

		str_ptr = Get_Control_String (SUBAREA_PLAN_FORMAT);

		if (str_ptr != NULL) {
			subarea_plans.File_Format (str_ptr);
			Print (1, "%s Format = %s", subarea_plans.File_Type (), str_ptr);
		}
		subarea_plans.Node_Based_Flag (type_flag);

		if (!subarea_plans.Open (0)) {
			File_Error ("Opening Subarea Plan File", subarea_plans.Filename ());
		}
		subarea_flag = true;
		if (subarea_plans.Node_Based_Flag () != type_flag) {
			Error ("Incompatible Plan Formats");
		}
	}

	//---- open the output plans ----

	str_ptr = Get_Control_String (OUTPUT_PLAN_FILE);

	if (str_ptr != NULL) {
		output_plans.File_Type ("Output Plan File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, output_plans.File_Type (), str_ptr);

		output_plans.Filename (str_ptr);
		output_plans.File_Access (CREATE);

		str_cpy (output_name, sizeof (output_name), output_plans.Pathname ());
		extend = output_plans.Extend ();
		output_flag = true;

		str_ptr = Get_Control_String (OUTPUT_PLAN_FORMAT);

		if (str_ptr != NULL) {
			output_plans.File_Format (str_ptr);
			Print (1, "%s Format = %s", output_plans.File_Type (), str_ptr);

			binary_flag = (output_plans.File_Format () == BINARY);
		}
		output_plans.Node_Based_Flag (type_flag);

	} else if (merge_flag) {
		Error ("Plan Merging Requires an Output File");
	} else if (subarea_flag) {
		Error ("Subarea Merging Requires an Output File");
	}

	//---- plan sort option ----

	str_ptr = Get_Control_String (PLAN_SORT_OPTION);

	if (str_ptr != NULL && output_flag) {
		Print (2, "Plan Sort Option = %s", str_ptr);

		if (str_cmp (str_ptr, "TRAVELER") == 0) {
			sort = TRAVELER_SORT;
		} else if (str_cmp (str_ptr, "TIME") == 0) {
			if (merge_flag) {
				Error ("Plan Merging must be performed in TRAVELER Sort");
			}
			sort = TIME_SORT;
		} else {
			Error ("Unrecognized Plan Sort Option = %s", str_ptr);
		}
		if (!input_sort_flag || input_sort != sort) {
			input_plans.Plan_Memory (ALLOCATE_MEMORY);
			input_plans.Plan_Sort (sort);
			sort_flag = true;
		}
		if (sort_flag && subarea_flag) {
			Error ("Sorting and Subarea Merging are Incompatible");
		}
	}

	//---- plan combine option ----

	str_ptr = Get_Control_String (PLAN_COMBINE_OPTION);

	if (str_ptr != NULL && parts_flag && output_flag) {
		Print (2, "Plan Combine Option = %s", str_ptr);

		if (!input_sort_flag && !sort_flag) {
			Error ("The Input Sort or Sort Option are needed for Plan Combining");
		}
		if (str_cmp (str_ptr, "FILE") == 0) {
			combine = FILE_COMBINE;
		} else if (str_cmp (str_ptr, "MEMORY") == 0) {
			if (merge_flag) {
				Error ("Plan Merging and Memory Combining are Incompatible");
			} else if (subarea_flag) {
				Error ("Subarea Merging and Memory Combining are Incompatible");
			}
			if (sort_flag) {
				combine = MEMORY_COMBINE;
			} else {
				combine = FILE_COMBINE;
			}
		} else {
			Error ("Unrecognized Plan Combine Option = %s", str_ptr);
		}
		combine_flag = true;
	}

	//---- max plan size ----

	str_ptr = Get_Control_String (MAX_OUTPUT_PLAN_SIZE);

	if (str_ptr != NULL && (combine_flag || (!parts_flag && output_flag))) {
		Print (2, "Maximum Output Plan Size = %s megabytes", str_ptr);

		max_size = atol (str_ptr);

		if (max_size < 1 || max_size > 2048) {
			Error ("Maximum Output Plan Size %d is Out of Range (1-2048)", max_size);
		}
		max_size *= 1024 * 1024;
		max_size_flag = true;
	}

	//---- get the select traveler range ----
	
	str_ptr = Get_Control_String (SELECT_TRAVELER_RANGE);

	if (str_ptr != NULL) {
		Print (2, "Select Traveler Range = %s", str_ptr);

		if (!hhold_range.Add_Ranges (str_ptr)) {
			Error ("Select Traveler Range", str_ptr);
		}
		hhold_flag = true;
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr != NULL) {
		Print (2, "Select Time Periods = %s", str_ptr);

		time_period.Format (Time_Step::HOURS);

		if (!time_period.Add_Ranges (str_ptr)) {
			File_Error ("Time Period Range", str_ptr);
		}
		time_flag = true;
	}

	//---- read the selection percentage ----

	str_ptr = Get_Control_String (SELECTION_PERCENTAGE);

	if (str_ptr != NULL) {
		if (input_sort != TRAVELER_SORT) {
			Error ("The Input Plans must be in Traveler order for Plan Selection");
		}
		Get_Double (str_ptr, &percent);

		if (percent < 0.1 || percent > 100.0) {
			Error ("Selection Percentage %.2lf is Out of Range (0.1-100.0)", percent);
		}
		Print (2, "Selection Percentage = %.2lf", percent);

		percent /= 100.0;
		select_flag = (percent != 1.0);
	}

	if ((time_flag || select_flag || hhold_flag) && subarea_flag) {
		Error ("Subarea Merging and Plan Selections are Incompatible");
	}

	//---- check plan partitions ----

	str_ptr = Get_Control_String (CHECK_PLAN_PARTITIONS);

	if (str_ptr != NULL) {
		check_flag = Get_Control_Flag (CHECK_PLAN_PARTITIONS);

		Print (2, "Check Plan Partitions = %s", str_ptr);

		if (check_flag) {
			partition.Initialize (sizeof (Partition));
		}
	}

	//---- update plan partitions ----

	str_ptr = Get_Control_String (UPDATE_PLAN_PARTITIONS);

	if (str_ptr != NULL) {
		partition_flag = Get_Control_Flag (UPDATE_PLAN_PARTITIONS);

		Print (2, "Update Plan Partitions = %s", str_ptr);
	}

	//---- get the household list file ----

	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List");
		hhlist_file.Filename (Project_Filename (str_ptr));

		Print_Filename (2, hhlist_file.File_Type (), str_ptr);

		if (!hhlist_file.Open (0)) {
			File_Error ("Opening Household List File", hhlist_file.Filename ());
		}
		hhlist_flag = true;
	} else if (partition_flag) {
		Error ("Household Lists are required for Partition Updates");
	}

	if (hhlist_flag) {
		combine_flag = false;

		if (sort_flag) {
			Error ("Plan Sorting and Household Lists are Incompatible");
		}
		if (merge_flag) {
			Error ("Plan Merging and Household Lists are Incompatible");
		} else if (subarea_flag) {
			Error ("Subarea Merging and Household Lists are Incompatible");
		}
		if (check_flag) {
			Error ("Checking and Updating Plan Partitions are Incompatible");
		}
		if (!output_flag) {
			Error ("Output Plans are Required for Household Lists");
		}
		partition.Initialize (sizeof (Partition));
	}

	//---- get the delete household list file ----

	str_ptr = Get_Control_String (DELETE_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		delete_file.File_Type ("Delete Household List");
		delete_file.Filename (Project_Filename (str_ptr));

		Print_Filename (2, delete_file.File_Type (), str_ptr);

		if (!delete_file.Open (0)) {
			File_Error ("Opening Delete Household List File", delete_file.Filename ());
		}
		delete_flag = true;
	}

	//---- get the delete one leg plans flag ----

	str_ptr = Get_Control_String (DELETE_ONE_LEG_PLANS);

	if (str_ptr != NULL) {
		one_leg_flag = Get_Control_Flag (DELETE_ONE_LEG_PLANS);

		Print (1, "Delete One Leg Plans = %s", str_ptr);
	}

	//---- get the fix plan file ----

	str_ptr = Get_Control_String (FIX_PLAN_FILE);

	if (str_ptr != NULL) {
		fix_file.File_Type ("Fix Plan File");

		if (!fix_file.Open (Project_Filename (str_ptr))) {
			File_Error ("Opening Fix Plan File", fix_file.Filename ());
		}
		fix_flag = true;
	}

	//---- merge by trip ----

	str_ptr = Get_Control_String (MERGE_BY_TRIP);

	if (str_ptr != NULL) {
		trip_flag = Get_Control_Flag (MERGE_BY_TRIP);

		Print (2, "Merge by Trip = %s", str_ptr);
	}

	//---- random number seed ----

	if (select_flag || fix_flag) {
		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (2, "Random Number Seed = %d", random.Seed ());
	}

	//---- check output file name ----

	if (output_flag) {
		output_plans.Plan_Sort (input_plans.Plan_Sort ());

		if (max_size_flag || !combine_flag) {
			extend = output_plans.Extend (input_plans.Extend ());
			if (extend == 0 && max_size_flag) {
				extend = output_plans.Extend (1);
			}
		}
		if (!hhlist_flag || !delete_flag) {
			if (!output_plans.Open (0)) {
				File_Error ("Creating Output Plan File", output_plans.Filename ());
			}
		}
		if (combine_flag && combine == FILE_COMBINE && (sort_flag || merge_flag || subarea_flag)) {

			//---- set the temporary output filename ----

			output_plans.Filename (Filename (output_name, TEMP_LABEL));
			output_plans.Extend (input_plans.Extend ());
			output_plans.File_Format (BINARY);
		}
	}
	
	//---- read report types ----

	compare_flag = path_flag = false;
	dump_path = dump_time = MAX_INTEGER;

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		if (i != FIX_PLAN) compare_flag = true;

		if (i == PATH_CHANGE || i == DUMP_PATH_CHANGES) {
			path_flag = true;
			if (i == DUMP_PATH_CHANGES) {
				dump = (int) (Report_Data () * NUM_CHANGE_BINS / 100.0 + 0.5);
				if (dump < dump_path) {
					dump_path = dump;
				}
			}
		} else if (i == DUMP_TIME_CHANGES) {
			dump = (int) (Report_Data () * NUM_CHANGE_BINS / 100.0 + 0.5);
			if (dump < dump_time) {
				dump_time = dump;
			}
		}
	}

	if (compare_flag) {
		if (!merge_flag) {
			Write (1);
			Warning ("Change Reports require a Merge Plan File");
			compare_flag = false;
		}
		memset (time_changes, '\0', sizeof (time_changes));
		memset (path_changes, '\0', sizeof (path_changes));

		increment = MIDNIGHT / TOTAL_TIME_PERIOD;
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
