//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TripPrep.hpp"

#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Vehicle_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TripPrep::Program_Control (void)
{
	int  i, num;
	char *str_ptr, *format_ptr;
	bool equiv_flag;

	equiv_flag = false;

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	location_flag = Network_File_Flag (ACTIVITY_LOCATION);
	
	merge_veh_flag = (Get_Control_String (MERGE_VEHICLE_FILE) != NULL && Demand_File_Flag (NEW_VEHICLE));

	//---- set the type flag for manual read ----

	trip_flag = Demand_File_Flag (TRIP);
	activity_flag = Demand_File_Flag (ACTIVITY);

	if (!trip_flag && !activity_flag) {
		Error ("A Trip or Activity file is required");
	}
	Demand_File_False (TRIP);
	Demand_File_False (ACTIVITY);

	create_flag = Get_Control_Flag (CREATE_INDEPENDENT_TRIPS);
	convert_flag = Get_Control_Flag (CONVERT_ACTIVITIES_TO_TRIPS);
	synod_flag = (Get_Control_String (ZONE_TARGET_FILE) != NULL);

	//---- replicate the output file ----

	if (activity_flag) {
		output_flag = Demand_File_Flag (NEW_ACTIVITY);

		if (output_flag) {
			Activity_File *file, *new_file;

			file = (Activity_File *) Demand_Db_Base (ACTIVITY);
			new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);

			new_file->Write_Header ();

			//----- open the merge file ----

			str_ptr = Get_Control_String (MERGE_ACTIVITY_FILE);

			if (str_ptr != NULL) {
				merge_act_file.File_Type ("Merge Activity File");
				Print (1);

				format_ptr = Get_Control_String (MERGE_ACTIVITY_FORMAT);

				if (format_ptr != NULL) {
					merge_act_file.Dbase_Format (format_ptr);
				}
				if (Partition ()) {
					merge_act_file.Filename (str_ptr, Partition_Number ());
				} else {
					merge_act_file.Filename (str_ptr);
				}
				Print_Filename (1, merge_act_file.File_Type (), merge_act_file.Groupname ());

				if (!merge_act_file.Open (0)) {
					File_Error (merge_act_file.File_Type (), merge_act_file.Filename ());
				}
				merge_act_flag = true;
			}
		}
	}
	if (trip_flag) {
		output_flag = Demand_File_Flag (NEW_TRIP);

		if (output_flag) {
			Trip_File *file, *new_file;

			file = (Trip_File *) Demand_Db_Base (TRIP);
			new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);

			new_file->Write_Header ();

			//----- open the merge file ----

			str_ptr = Get_Control_String (MERGE_TRIP_FILE);

			if (str_ptr != NULL) {
				merge_trip_file.File_Type ("Merge Trip File");
				Print (1);

				format_ptr = Get_Control_String (MERGE_TRIP_FORMAT);

				if (format_ptr != NULL) {
					merge_trip_file.Dbase_Format (format_ptr);
				}
				if (Partition ()) {
					merge_trip_file.Filename (str_ptr, Partition_Number ());
				} else {
					merge_trip_file.Filename (str_ptr);
				}
				Print_Filename (1, merge_trip_file.File_Type (), merge_trip_file.Groupname ());

				if (!merge_trip_file.Open (0)) {
					File_Error (merge_trip_file.File_Type (), merge_trip_file.Filename ());
				}
				merge_trip_flag = true;
			}
		}
	}

	//---- check for vehicle processing ----

	if (create_flag || synod_flag) {
		if (!Demand_File_Flag (VEHICLE) || !Demand_File_Flag (NEW_VEHICLE)) {
			Error ("Input and Output Vehicle Files are required for Independent Trips");
		}
	} else if (output_flag || merge_veh_flag) {
		if (Demand_File_Flag (NEW_VEHICLE)) {
			vehicle_flag = true;
		}
		if (vehicle_flag) {
			if (!Demand_File_Flag (VEHICLE)) {
				Error ("A Vehicle File is required");
			}
			Vehicle_File *file, *new_file;

			file = (Vehicle_File *) Demand_Db_Base (VEHICLE);
			new_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);

			new_file->Write_Header ();

			//----- open the merge file ----

			str_ptr = Get_Control_String (MERGE_VEHICLE_FILE);

			if (str_ptr != NULL) {
				merge_veh_file.File_Type ("Merge Vehicle File");
				Print (1);

				format_ptr = Get_Control_String (MERGE_VEHICLE_FORMAT);

				if (format_ptr != NULL) {
					merge_veh_file.Dbase_Format (format_ptr);
				}
				if (!merge_veh_file.Open (Project_Filename (str_ptr))) {
					Error ("Opening %s", merge_veh_file.File_Type ());
				}
				merge_veh_flag = true;
			}
		}
		Demand_File_False (VEHICLE);
	}

	//---- open the household list file ----
	
	str_ptr = Get_Control_String (HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		hhlist_file.File_Type ("Household List");
						
		str_ptr = Project_Filename (str_ptr);

		if (Partition ()) {
			hhlist_file.Filename (str_ptr, Partition_Number ());
		} else {
			hhlist_file.Filename (str_ptr);
		}
		Print_Filename (2, hhlist_file.File_Type (), hhlist_file.Groupname ());

		if (!hhlist_file.Open (0)) {
			File_Error (hhlist_file.File_Type (), hhlist_file.Filename ());
		}
		hhlist_flag = true;

		if (output_flag && hhlist_file.Extend ()) {
			if (activity_flag) {
				Activity_File *file, *new_file;

				file = (Activity_File *) Demand_Db_Base (ACTIVITY);
				new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

				if (!file->Extend ()) {
					if (!new_file->Extend ()) {
						Error ("Partitioned Household Lists are Illogical");
					}
					split_flag = true;
				}
			}
			if (trip_flag) {
				Trip_File *file, *new_file;

				file = (Trip_File *) Demand_Db_Base (TRIP);
				new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

				if (!file->Extend ()) {
					if (!new_file->Extend ()) {
						Error ("Partitioned Household Lists are Illogical");
					}
					split_flag = true;
				}
			}
		}
	}

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_periods.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- select households ----

	str_ptr = Get_Control_String (SELECT_HOUSEHOLDS);

	if (str_ptr != NULL) {
		Print (2, "Select Households = %s", str_ptr);

		if (!traveler_range.Add_Ranges (str_ptr)) {
			Error ("Illegal Households Range");
		}
		traveler_flag = true;
	}

	//---- get the select time periods ----
	
	str_ptr = Get_Control_String (SELECT_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Select Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}

	//---- get the select trip modes ----

	str_ptr = Get_Control_String (SELECT_TRIP_MODES);

	if (str_ptr != NULL) {

		Print (2, "Select Trips for Modes = %s", str_ptr);

		memset (trip_mode, '\0', MAX_MODE * sizeof (bool));
		num = 0;
		Data_Range range;
		Range_Data *ptr;

		range.Add_Ranges (str_ptr);

		for (ptr = range.First (); ptr; ptr = range.Next ()) {
			for (i=ptr->Low (); i <= ptr->High (); i++) {
				if (i < 1 || i >= MAX_MODE) {
					Error ("Selected Mode %d is Out of Range (1..%d)", i, MAX_MODE-1);
				}
				trip_mode [i] = true;
				num++;
			}
		}
		if (num == 0) {
			Error ("No Modes were Selected for Processing");
		}
	} else {
		for (i=1; i < MAX_MODE; i++) {
			trip_mode [i] = true;
		}
	}

	//---- select trip purposes ----

	str_ptr = Get_Control_String (SELECT_TRIP_PURPOSES);

	if (str_ptr != NULL) {
		Print (2, "Select Trips for Purposes = %s", str_ptr);

		if (!purpose_range.Add_Ranges (str_ptr)) {
			Error ("Illegal Purpose Range");
		}
		purpose_flag = true;
	}

	//---- select origin zones ----

	str_ptr = Get_Control_String (SELECT_ORIGIN_ZONES);

	if (str_ptr != NULL) {
		Print (2, "Select Origin Zones = %s", str_ptr);

		if (!org_zone_range.Add_Ranges (str_ptr)) {
			Error ("Illegal Zone Range");
		}
		org_zone_flag = true;

		if (!location_flag) goto loc_error;
	}

	//---- select destination zones ----

	str_ptr = Get_Control_String (SELECT_DESTINATION_ZONES);

	if (str_ptr != NULL) {
		Print (2, "Select Destination Zones = %s", str_ptr);

		if (!des_zone_range.Add_Ranges (str_ptr)) {
			Error ("Illegal Zone Range");
		}
		des_zone_flag = true;

		if (!location_flag) goto loc_error;
	}

	//---- get the select origin polygon ----

	str_ptr = Get_Control_String (SELECT_ORIGIN_POLYGON);

	if (str_ptr != NULL) {
		Print (1);
		select_origin.File_Type ("Select Origin Polygon");

		select_origin.Open (Project_Filename (str_ptr));
		
		if (!select_origin.Read_Record ()) {
			Error ("Reading %s", select_origin.File_Type ());
		}
		select_org_flag = true;

		if (!location_flag) goto loc_error;
	}

	//---- get the select destination polygon ----

	str_ptr = Get_Control_String (SELECT_DESTINATION_POLYGON);

	if (str_ptr != NULL) {
		Print (1);
		select_destination.File_Type ("Select Destination Polygon");

		select_destination.Open (Project_Filename (str_ptr));
		
		if (!select_destination.Read_Record ()) {
			Error ("Reading %s", select_destination.File_Type ());
		}
		select_des_flag = true;

		if (!location_flag) goto loc_error;
	}

	//---- get the select probability file ----

	str_ptr = Get_Control_String (SELECT_PROBABILITY_FILE);

	if (str_ptr != NULL) {
		Print (1);
		factor_file.File_Type ("Select Probability File");

		format_ptr = Get_Control_String (SELECT_PROBABILITY_FORMAT);

		if (format_ptr != NULL) {
			factor_file.Dbase_Format (format_ptr);
		}
		if (!factor_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", factor_file.File_Type ());
		}
		factor_flag = true;
		
		//---- open the zone equivalence ----

		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr != NULL) {
			zone_equiv.Open (Project_Filename (str_ptr));
			zone_equiv_flag = true;
		}

		str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE);

		if (str_ptr != NULL) {
			time_equiv.File_Type ("Time Period Equivalence");

			if (!time_equiv.Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", time_equiv.File_Type ());
			}
			equiv_flag = true;
		}
	}

	//---- get the selection probability ----

	str_ptr = Get_Control_String (SELECTION_PERCENTAGE);

	if (str_ptr != NULL) {
		probability = atof (str_ptr);
		Print (2, "Selection Percentage = %.2lf%%", probability);

		if (probability < 0.0 || probability > 100.0) {
			Error ("Selection Probability %.2lf is Out of Range (0.0..100.0)", probability);
		}
		prob_flag = true;
		probability /= 100.0;
	}

	//---- random number seed ----

	if (prob_flag || factor_flag || synod_flag) {
		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (2, "Random Number Seed = %d", random.Seed ());

		//---- check for required data ----

		if (factor_flag || synod_flag) {
			if (!location_flag) goto loc_error;

			if (!Network_Db_Base (ACTIVITY_LOCATION)->Required_Field ("ZONE", "TAZ")) {
				Error ("The Zone Field was Not Found on the Activity Location File");
			}
		}
		if (prob_flag) {
			random2.Seed (random.Seed () + 1);
		}
	}

	//---- get start time shift ----

	str_ptr = Get_Control_String (START_TIME_SHIFT);

	if (str_ptr != NULL) {
		shift_size = time_periods.Duration (str_ptr);

		Print (2, "Start Time Shift = %s", time_periods.Format_Duration (shift_size));

		shift_flag = true;
	}

	//---- read data processing script ----

	str_ptr = Get_Control_String (DATA_PROCESSING_SCRIPT);

	if (str_ptr != NULL) {
		if (!output_flag) {
			Error ("An Output File is Required for Data Processing Scripts");
		}
		script_flag = true;
		Print (1);
		program_file.File_Type ("Data Processing Script");
		program_file.Open (Project_Filename (str_ptr));
	}

	//---- output all records ----

	str_ptr = Get_Control_String (OUTPUT_ALL_RECORDS);

	if (str_ptr != NULL) {
		if (!output_flag) {
			Error ("An Output File is Required to Output All Records");
		}
		Print (2, "Output All Records = %s", str_ptr);
		all_flag = Get_Control_Flag (OUTPUT_ALL_RECORDS);
	}

	//---- create independent trips ----
	
	str_ptr = Get_Control_String (CREATE_INDEPENDENT_TRIPS);

	if (str_ptr != NULL) {
		Print (2, "Create Independent Trips = %s", str_ptr);
	}
	if (create_flag) {
		if (!activity_flag && !trip_flag) {
			Error ("Input Trip/Activity File is required for Independent Trips");
		}
		if (!Demand_File_Flag (NEW_TRIP)) {
			Error ("A New Trip File is required for Independent Trips");
		}
		if (!Network_File_Flag (ACTIVITY_LOCATION)) {
			Error ("An Activity Location Table is required for Independent Trips");
		}
		if (!Network_File_Flag (PROCESS_LINK)) {
			Error ("A Process Link Table is required for Independent Trips");
		}

		//---- starting household id ----

		str_ptr = Get_Control_String (STARTING_HOUSEHOLD_ID);

		if (str_ptr != NULL) {
			Print (2, "Starting Household ID = %s", str_ptr);

			hhold_id = atoi (str_ptr);

			if (hhold_id < 1) {
				Error ("Starting Household ID %d is Out of Range", hhold_id);
			}
		}

		//---- starting vehicle id ----

		str_ptr = Get_Control_String (STARTING_VEHICLE_ID);

		if (str_ptr != NULL) {
			Print (2, "Starting Vehicle ID = %s", str_ptr);

			veh_id = atoi (str_ptr);

			if (veh_id < 1) {
				Error ("Starting Vehicle ID %d is Out of Range", veh_id);
			}
		}
	}

	//---- open the new household list ----

	if (create_flag || prob_flag) {
		str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

		if (str_ptr != NULL) {
			newhh_file.File_Type ("New Household List");
			newhh_file.File_Access (CREATE);
							
			str_ptr = Project_Filename (str_ptr);

			if (Partition ()) {
				newhh_file.Filename (str_ptr, Partition_Number ());
			} else {
				newhh_file.Filename (str_ptr);
			}
			Print_Filename (2, newhh_file.File_Type (), newhh_file.Groupname ());

			if (!newhh_file.Open (0)) {
				File_Error (newhh_file.File_Type (), newhh_file.Filename ());
			}
			newhh_flag = true;
		}
	}

	//---- get the zone target file ----

	if (synod_flag) {
		str_ptr = Get_Control_String (ZONE_TARGET_FILE);

		zone_file.File_Type ("Zone Target File");

		zone_file.Open (Project_Filename (str_ptr));

		//---- target household range ----

		str_ptr = Get_Control_String (TARGET_HOUSEHOLD_RANGE);

		if (str_ptr != NULL) {
			Print (1, "Target Household Range = %s", str_ptr);

			if (!hhold_range.Add_Ranges (str_ptr)) {
				Error ("Illegal Household Range");
			}
		} else {
			hhold_range.Add_Range (1, MAX_INTEGER);
		}
	}

	//---- move origin file ----

	str_ptr = Get_Control_String (MOVE_ORIGIN_FILE);

	if (str_ptr != NULL) {
		move_org_file.File_Type ("Move Origin File");

		move_org_file.Open (Project_Filename (str_ptr));
		move_org_flag = true;
		int num, rec [2];

		while (move_org_file.Read ()) {
			str_ptr = move_org_file.Record ();

			str_ptr = Get_Integer (str_ptr, &num);
			if (num == 0) continue;

			rec [0] = num;

			Get_Integer (str_ptr, &num);
			if (num == 0) continue;

			rec [1] = num;

			if (!move_org_map.Add (rec)) {
				Error ("Adding Move Origin Data");
			}
		}
	}

	//---- move destination file ----

	str_ptr = Get_Control_String (MOVE_DESTINATION_FILE);

	if (str_ptr != NULL) {
		move_des_file.File_Type ("Move Destination File");

		move_des_file.Open (Project_Filename (str_ptr));
		move_des_flag = true;
		int num, rec [2];

		while (move_des_file.Read ()) {
			str_ptr = move_des_file.Record ();

			str_ptr = Get_Integer (str_ptr, &num);
			if (num == 0) continue;

			rec [0] = num;

			Get_Integer (str_ptr, &num);
			if (num == 0) continue;

			rec [1] = num;

			if (!move_des_map.Add (rec)) {
				Error ("Adding Move Destination Data");
			}
		}
	}

	//---- open the new household list ----

	if ((move_org_flag || move_des_flag) && !newhh_flag) {
		str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

		if (str_ptr != NULL) {
			newhh_file.File_Type ("New Household List");
			newhh_file.File_Access (CREATE);
							
			str_ptr = Project_Filename (str_ptr);

			if (Partition ()) {
				newhh_file.Filename (str_ptr, Partition_Number ());
			} else {
				newhh_file.Filename (str_ptr);
			}
			Print_Filename (2, newhh_file.File_Type (), newhh_file.Groupname ());

			if (!newhh_file.Open (0)) {
				File_Error (newhh_file.File_Type (), newhh_file.Filename ());
			}
			newhh_flag = true;
		}
	}

	//---- convert activities to trips ----
	
	if (convert_flag) {
		str_ptr = Get_Control_String (CONVERT_ACTIVITIES_TO_TRIPS);

		Print (2, "Convert Activities to Trips = %s", str_ptr);

		if (!activity_flag) {
			Error ("Input Activity File is required for Conversion");
		}
		if (!Demand_File_Flag (NEW_TRIP)) {
			Error ("A New Trip File is required for Conversion");
		}
	}

	//---- read report types ----

	List_Reports ();
	
	if (factor_flag) {
		if (zone_equiv_flag) {
			zone_equiv.Read (Report_Flag (ZONE_EQUIV));
		}
		if (equiv_flag) {
			time_equiv.Read (Report_Flag (TIME_EQUIV));
		}
	}
	return;

loc_error:
	Error ("Activity Location File is required for processing");
} 
