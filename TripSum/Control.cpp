//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TripSum.hpp"

#include "Activity_File.hpp"
#include "Trip_File.hpp"
#include "Vehicle_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TripSum::Program_Control (void)
{
	int  i, num, length;
	char *str_ptr, *format_ptr;
	bool period_flag, equiv_flag;

	Range_Data *range_ptr;
	equiv_flag = false;

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	location_flag = Network_File_Flag (ACTIVITY_LOCATION);

	//---- set the type flag for manual read ----

	trip_flag = Demand_File_Flag (TRIP);
	activity_flag = Demand_File_Flag (ACTIVITY);

	if (!trip_flag && !activity_flag) {
		Error ("A Trip or Activity file is required");
	}
	Demand_File_False (TRIP);
	Demand_File_False (ACTIVITY);

	create_flag = Get_Control_Flag (CREATE_INDEPENDENT_TRIPS);
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
		}
	}

	//---- check for vehicle processing ----

	if (create_flag || synod_flag) {
		if (!Demand_File_Flag (VEHICLE) || !Demand_File_Flag (NEW_VEHICLE)) {
			Error ("Input and Output Vehicle Files are required for Independent Trips");
		}
	} else if (output_flag) {
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

	//---- household person count file ----

	str_ptr = Get_Control_String (HOUSEHOLD_PERSON_COUNT);

	if (str_ptr != NULL) {
		count_file.File_Type ("Household Person Count");
						
		str_ptr = Project_Filename (str_ptr);

		if (Partition ()) {
			count_file.Filename (str_ptr, Partition_Number ());
		} else {
			count_file.Filename (str_ptr);
		}
		Print_Filename (2, count_file.File_Type (), count_file.Groupname ());

		if (!count_file.Open (0)) {
			File_Error (count_file.File_Type (), count_file.Filename ());
		}
		count_flag = true;
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

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_INCREMENT);
	period_flag = true;

	if (str_ptr != NULL) {
		increment = atol (str_ptr);

		if (increment < 0 || increment > 240) {
			Error ("Summary Time Increment %d is Out of Range (0-240)", increment);
		}
		increment *= 60;
	}
	if (increment == 0) {
		Time_Increment (MIDNIGHT);
	} else {
		time_periods.Period_Flag (true);
		Time_Increment (increment);
	}
	time_periods.Increment (increment);

	//---- get the summary time periods ----
	
	str_ptr = Get_Control_String (SUMMARY_TIME_PERIODS);

	if (str_ptr == NULL) {
		str_ptr = (char *) "All";
	}
	Print (2, "Summary Time Periods = %s", str_ptr);

	if (!time_periods.Add_Ranges (str_ptr)) {
		File_Error ("Time Period Range", str_ptr);
	}
	num_inc = time_periods.Num_Ranges ();

	ttime_data.Periods (num_inc);

	Print (1, "Summary Time Increment = %d minutes", (increment / 60));

	//---- get summary increment ----
	
	str_ptr = Get_Control_String (SUMMARY_LENGTH_INCREMENT);
	length = 500;

	if (str_ptr != NULL) {
		length = atol (str_ptr);

		if (length < 10 || length > 2000) {
			Error ("Summary Length Increment %d is Out of Range (10-2000)", length);
		}
	}
	Print (2, "Summary Length Increment = %d meters", length);

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

		str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE);

		if (str_ptr != NULL) {
			time_equiv.File_Type ("Time Period Equivalence");

			if (!time_equiv.Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", time_equiv.File_Type ());
			}
			equiv_flag = true;
		}
	}

	//---- random number seed ----

	if (factor_flag || synod_flag) {
		str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

		if (str_ptr != NULL) {
			random.Seed (atoi (str_ptr));
		}
		Print (1, "Random Number Seed = %d", random.Seed ());

		//---- check for required data ----

		if (!location_flag) goto loc_error;

		if (!Network_Db_Base (ACTIVITY_LOCATION)->Required_Field ("ZONE", "TAZ")) {
			Error ("The Zone Field was Not Found on the Activity Location File");
		}
	}

	//---- create output volume file ----

	str_ptr = Get_Control_String (NEW_LINK_TRIP_END_FILE);

	if (str_ptr != NULL) {
		Print (1);
		volume_file.File_Type ("Link Trip End File");

		format_ptr = Get_Control_String (NEW_LINK_TRIP_END_FORMAT);

		if (format_ptr != NULL) {
			volume_file.Dbase_Format (format_ptr);
		}		
		volume_file.Create (Project_Filename (str_ptr, Extension ()));

		for (range_ptr = time_periods.First (); range_ptr != NULL; range_ptr = time_periods.Next ()) {
			volume_file.Add (range_ptr);
		}
		volume_file.Create_Fields ();
		volume_file.Write_Header ();

		volume_flag = link_flag = true;
	}

	//---- get the time distribution file ----

	str_ptr = Get_Control_String (NEW_TIME_DISTRIBUTION);

	if (str_ptr != NULL) {
		time_file.File_Type ("New Time Distribution");
		Print (1);

		time_file.Create (Project_Filename (str_ptr, Extension ()));
		time_flag = true;
	}

	//---- get the trip table file ----

	str_ptr = Get_Control_String (NEW_TRIP_TABLE_FILE);

	if (str_ptr != NULL) {
		Print (1);

		//---- check for required data ----

		if (!location_flag) goto loc_error;

		if (!Network_Db_Base (ACTIVITY_LOCATION)->Required_Field ("ZONE", "TAZ")) {
			Error ("The Zone Field was Not Found on the Activity Location File");
		}

		//---- check the number of increments ----

		if (!trip_table.Set_Key (num_inc)) {
			Error ("Trip Table Index Limits Exceeded");
		}
	
		//---- create the file ----

		format_ptr = Get_Control_String (NEW_TRIP_TABLE_FORMAT);

		if (format_ptr != NULL) {
			trip_file.Dbase_Format (format_ptr);
		}
		trip_file.File_Type ("Trip Table File");
		trip_file.Period_Flag (num_inc > 1);

		trip_file.Create (Project_Filename (str_ptr, Extension ()));
		table_flag = true;

		//---- open the zone equivalence ----

		str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

		if (str_ptr != NULL) {
			Print (1);
			zone_equiv.Open (Project_Filename (str_ptr));
			zone_flag = true;
		}
	}

	//---- open the trip time file ----

	str_ptr = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (str_ptr != NULL) {
		if (trip_time_length.Open_Trip_Length_File (str_ptr, "New Trip Time File")) {
			time_length_flag = true;
			trip_time_length.Increment (60);	//---- minutes ----
		}
	}

	//---- open the trip distance file ----

	str_ptr = Get_Control_String (NEW_TRIP_DISTANCE_FILE);

	if (str_ptr != NULL) {
		if (trip_distance_length.Open_Trip_Length_File (str_ptr, "New Trip Distance File")) {
			distance_length_flag = true;
			trip_distance_length.Increment (length);	//---- meters ----
		}
		if (!location_flag) goto loc_error;
	}

	//---- get the distance calculation ----

	str_ptr = Get_Control_String (DISTANCE_CALCULATION);

	if (str_ptr != NULL) {
		distance_method = Distance_Code (str_ptr);
	}
	Print (2, "Distance Calculation = %s", Distance_Code (distance_method));

	//---- create location trip end file ----

	str_ptr = Get_Control_String (NEW_LOCATION_TRIP_END_FILE);

	if (str_ptr != NULL) {
		if (!location_flag) goto loc_error;

		Print (1);
		loc_end_file.File_Type ("Location Trip End File");

		format_ptr = Get_Control_String (NEW_LOCATION_TRIP_END_FORMAT);

		if (format_ptr != NULL) {
			loc_end_file.Dbase_Format (format_ptr);
		}		
		loc_end_file.Create (Project_Filename (str_ptr, Extension ()));

		for (range_ptr = time_periods.First (); range_ptr != NULL; range_ptr = time_periods.Next ()) {
			loc_end_file.Add (range_ptr);
		}
		loc_end_file.Create_Fields ();
		loc_end_file.Write_Header ();

		loc_end_flag = true;

		loc_end_data.Time_Periods (&time_periods);
	}

	//---- create zone trip end file ----

	str_ptr = Get_Control_String (NEW_ZONE_TRIP_END_FILE);

	if (str_ptr != NULL) {
		if (!location_flag) goto loc_error;

		Print (1);
		zone_end_file.File_Type ("Zone Trip End File");

		format_ptr = Get_Control_String (NEW_ZONE_TRIP_END_FORMAT);

		if (format_ptr != NULL) {
			zone_end_file.Dbase_Format (format_ptr);
		}		
		zone_end_file.Create (Project_Filename (str_ptr, Extension ()));

		for (range_ptr = time_periods.First (); range_ptr != NULL; range_ptr = time_periods.Next ()) {
			zone_end_file.Add (range_ptr);
		}
		zone_end_file.Create_Fields ();
		zone_end_file.Write_Header ();

		zone_end_flag = true;

		zone_end_data.Time_Periods (&time_periods);
	}

	//---- create independent trips ----
	
	str_ptr = Get_Control_String (CREATE_INDEPENDENT_TRIPS);

	if (str_ptr != NULL) {
		Print (2, "Create Independent Trips = %s", str_ptr);
	}
	if (create_flag) {
		if (!activity_flag) {
			Error ("Input Activity File is required for Independent Trips");
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

		//---- open the new household list ----

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

	//---- read report types ----

	List_Reports ();

	if (!link_flag) {
		link_flag = (Report_Flag (TOP_100_LINKS) || Report_Flag (TOP_100_LANES));
	}
	if (link_flag && !Network_File_Flag (LINK)) {
		Error ("A Link File is Required for Link-Based Output");
	}
	if (link_flag && !location_flag) goto loc_error;

	//---- process support data ----

	if (zone_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	if (factor_flag && equiv_flag) {
		time_equiv.Read (Report_Flag (TIME_EQUIV));
	}

	//---- allocate work space ----

	if (time_flag || Report_Flag (TRIP_TIME)) {
		time_flag = true;

		if (!start_time.Num_Records (num_inc)) goto mem_error;
		if (!end_time.Num_Records (num_inc)) goto mem_error;
		if (!mid_time.Num_Records (num_inc)) goto mem_error;
	}
	length_flag = Report_Flag (TRIP_LENGTH);
	trip_purpose_flag = Report_Flag (TRIP_PURPOSE);
	mode_flag = Report_Flag (MODE_LENGTH);
	mode_purpose_flag = Report_Flag (MODE_PURPOSE);

	if (length_flag && !location_flag) goto loc_error;

	return;

	//---- error message ----

mem_error:
	Error ("Insufficient Memory for %d Point Diurnal Distribution", num);

loc_error:
	Error ("Activity Location File is required for processing");
} 
