//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ConvertTrips.hpp"

#include "Db_Base.hpp"
#include "Matrix_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ConvertTrips::Program_Control (void)
{
	int i, j, num_file, lvalue;
	double dvalue;
	bool factor_flag;
	char *str_ptr, buffer [FIELD_BUFFER], *format_ptr;

	File_Group *group;

	char *time_control [] = {"ORIGIN", "DESTINATION", "MID-TRIP", NULL};

	factor_flag = false;

	//---- open demand and network files ----

	Demand_Service::Program_Control ();

	//---- set the file points ----

	zone_flag = Network_File_Flag (ZONE);

	trip_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);
	household_file = (Household_File *) Demand_Db_Base (NEW_HOUSEHOLD);
	population_file = (Population_File *) Demand_Db_Base (NEW_POPULATION);
	vehicle_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

	//---- check for copy commands ----

	if (Demand_File_Flag (TRIP)) {
		trip_file->Clear_Fields ();
		trip_file->Replicate_Fields (Demand_Db_Base (TRIP));
		trip_file->Write_Header ();
	}
	if (Demand_File_Flag (HOUSEHOLD)) {
		household_file->Clear_Fields ();
		household_file->Replicate_Fields (Demand_Db_Base (HOUSEHOLD));
		household_file->Write_Header ();
	}
	if (Demand_File_Flag (POPULATION)) {
		population_file->Clear_Fields ();
		population_file->Replicate_Fields (Demand_Db_Base (POPULATION));
		population_file->Write_Header ();
	}
	if (Demand_File_Flag (VEHICLE)) {
		vehicle_file->Clear_Fields ();
		vehicle_file->Replicate_Fields (Demand_Db_Base (VEHICLE));
		vehicle_file->Write_Header ();
	}

	//---- get the starting hhold id ----
	
	str_ptr = Get_Control_String (STARTING_HOUSEHOLD_ID);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &hhold_id);
		if (hhold_id < 1) {
			Error ("Starting Household ID %d is Out of Range", hhold_id);
		}
	} else {
		hhold_id = 1;
	}
    Print (2, "Starting Household ID = %d", hhold_id);

	//---- get the starting vehicle id ----
	
	str_ptr = Get_Control_String (STARTING_VEHICLE_ID);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &vehicle_id);
		if (vehicle_id < 1) {
			Error ("Starting Vehicle ID %d is Out of Range", vehicle_id);
		}
	} else {
		vehicle_id = 1;
	}
	Print (1, "Starting Vehicle ID = %d", vehicle_id);

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!trip_time.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- get the additional travel time ----
	
	str_ptr = Get_Control_String (ADDITIONAL_TRAVEL_TIME);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &additional_time);

		if (additional_time < 0) {
			Error ("Additional Travel Time %d is Out of Range", additional_time);
		}
	}
	Print (2, "Additional Travel Time = %d seconds", additional_time);

	//---- open the zone location map ----

	str_ptr = Get_Control_String (ZONE_LOCATION_MAP_FILE);

	if (str_ptr != NULL) {
		zone_loc_flag = true;
		Print (1);

		zone_loc_map.Open (Project_Filename (str_ptr));
	}

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random_org.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random_org.Seed ());
	
	random_des.Seed (random_org.Seed ()+1);
	random_tod.Seed (random_org.Seed ()+2);

	//---- open the zone equivalence ----

	str_ptr = Get_Control_String (ZONE_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		equiv_flag = true;
		Print (1);

		zone_equiv.Open (Project_Filename (str_ptr));
	}

	//---- open the travel time skim ----

	str_ptr = Get_Control_String (TRAVEL_TIME_FILE);

	if (str_ptr != NULL) {
		skim_flag = true;
		Print (1);

		skim_file.Open (Project_Filename (str_ptr));
	}

	//---- write the report names ----

	List_Reports ();

	//---- get the number of files ----

	num_file = Highest_Control_Group (TRIP_TABLE_FILE_x, 0);

	if (num_file == 0) {
		Error ("No Trip Tables were Found");
	}

	//---- allocate file space ----

	if (!file_group.Max_Records (num_file)) {
		Error ("Insufficient Memory for %d Trip Tables", num_file);
	}

	//---- process each file ----

	for (i=1; i <= num_file; i++) {
		
		str_ptr = Get_Control_String (TRIP_TABLE_FILE_x, i);

		if (str_ptr == NULL) continue;

		if (!Break_Check (16)) {
			Print (1);
		}
		group = file_group.New_Record (true);

		group->Group (i);

		group->Trip_File (new Matrix_File ());

		str_fmt (buffer, sizeof (buffer), "Trip Table File #%d", i);

		group->Trip_File ()->File_Type (buffer);

		format_ptr = Get_Control_String (TRIP_TABLE_FORMAT_x, i);

		if (format_ptr != NULL) {
			group->Trip_File ()->Dbase_Format (format_ptr);
		}
		group->Trip_File ()->Open (Project_Filename (str_ptr));

		//---- get the trip scaling factor ----

		str_ptr = Get_Control_String (TRIP_SCALING_FACTOR_x, i);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);
			
			if (dvalue < 0.001 || dvalue > 100.0) {
				Error ("Trip Scaling Factor %.2lf is Out of Range (0.001..100.0)", dvalue);
			}
			group->Scaling_Factor (dvalue);
			Print (1, "Trip Scaling Factor = %.3lf", dvalue);
		} else {
			group->Scaling_Factor (1.0);
		}

		//---- get the trip time file ----

		str_ptr = Get_Control_String (TRIP_TIME_FILE_x, i);

		if (str_ptr == NULL) goto control_error;

		group->Time_File (new Diurnal_File ());

		str_fmt (buffer, sizeof (buffer), "Trip Time File #%d", i);

		group->Time_File ()->File_Type (buffer);

		format_ptr = Get_Control_String (TRIP_TIME_FORMAT_x, i);

		if (format_ptr != NULL) {
			group->Time_File ()->Dbase_Format (format_ptr);
		}
		group->Time_File ()->Open (Project_Filename (str_ptr));

		//---- get the trip time script -----

		str_ptr = Get_Control_String (TRIP_TIME_SCRIPT_x, i);

		if (str_ptr != NULL) {
			group->Script_File (new Db_File ());

			str_fmt (buffer, sizeof (buffer), "Trip Time Script #%d", i);

			group->Script_File ()->File_Type (buffer);

			group->Script_File ()->Open (Project_Filename (str_ptr));

		} else {

			//---- get the trip time field ----

			str_ptr = Get_Control_String (TRIP_TIME_FIELD_x, i);

			if (str_ptr != NULL) {
				if (!group->Time_Field (str_ptr)) {
					Error ("Trip Time Field %s was Not Found", str_ptr);
				}
				Print (1, "Trip Time Field Name = %s, Number = %d", str_ptr, group->Time_Field ());
			}
		}

		//---- get the time period range ----

		str_ptr = Get_Control_String (TIME_PERIOD_RANGE_x, i);

		if (str_ptr != NULL) {
			Print (1, "Time Period Range = %s", str_ptr);

			if (!group->Time_Periods (str_ptr)) {
				Error ("Time Period Range %s is Illogical", str_ptr);
			}
		}

		//---- get the time control point ----

		str_ptr = Get_Control_String (TIME_CONTROL_POINT_x, i);

		if (str_ptr != NULL) {
			for (j=0; time_control [j] != NULL; j++) {
				if (str_cmp (str_ptr, time_control [j]) == 0) break;
			}
			if (time_control [j] == NULL) {
				Error ("Time Control Point %s was Unrecognized", str_ptr);
			}
			group->Method (j);
		} else {
			group->Method (0);
		}
		Print (1, "Time Control Point = %s", time_control [group->Method ()]);

		//---- get the return trip offset ----

		str_ptr = Get_Control_String (RETURN_TRIP_OFFSET_x, i);

		if (str_ptr != NULL) {
			lvalue = trip_time.Duration (str_ptr);

			if (lvalue < 0 || lvalue >= MIDNIGHT) {
				Error ("Return Time Offset %s is Out of Range", str_ptr);
			}
			group->Duration (lvalue);

			Print (1, "Return Trip Offset = %s", trip_time.Format_Duration (lvalue));

			if (trip_time.Format () == Time_Step::SECONDS) {
				Print (0, " seconds");
			} else if (trip_time.Format () == Time_Step::HOURS) {
				Print (0, " hours");
			}
		} else {
			group->Duration (0);
		}

		//---- get the origin weight ----

		str_ptr = Get_Control_String (ORIGIN_WEIGHT_FIELD_x, i);

		if (str_ptr != NULL) {
			group->Org_Wt (Network_Db_Base (ACTIVITY_LOCATION)->Field_Number (str_ptr));

			if (group->Org_Wt () == 0) {
				Error ("Origin Weight Field %s was Not Found in the Activity Location File", str_ptr);
			}
			Print (1, "Origin Weight Field Name = %s, Number = %d", str_ptr, group->Org_Wt ());
		} else {
			group->Org_Wt (0);
			Print (1, "No Origin Weight Field --> Equal Weights");
		}

		//---- get the destination weight ----

		str_ptr = Get_Control_String (DESTINATION_WEIGHT_FIELD_x, i);

		if (str_ptr != NULL) {
			group->Des_Wt (Network_Db_Base (ACTIVITY_LOCATION)->Field_Number (str_ptr));

			if (group->Des_Wt () == 0) {
				Error ("Destination Weight Field %s was Not Found in the Activity Location File", str_ptr);
			}
			Print (1, "Destination Weight Field Name = %s, Number = %d", str_ptr, group->Des_Wt ());
		} else {
			group->Des_Wt (0);
			Print (1, "No Destination Weight Field --> Equal Weights");
		}

		//---- get the distance weight ----

		str_ptr = Get_Control_String (DISTANCE_WEIGHT_FLAG_x, i);

		if (str_ptr != NULL) {
			group->Dist_Wt (Get_Control_Flag (DISTANCE_WEIGHT_FLAG_x, i));

			Print (1, "Distance Weight Flag = %s", str_ptr);
		} else {
			group->Dist_Wt (true);
		}

		//---- get the trip purpose ----

		str_ptr = Get_Control_String (TRIP_PURPOSE_CODE_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);
			
			if (lvalue < 0 || lvalue > 100) {
				Error ("Trip Purpose %d is Out of Range (0..100)", lvalue);
			}
			group->Purpose (lvalue);
		} else {
			group->Purpose (1);
		}
		Print (1, "Trip Purpose Code = %d", group->Purpose ());

		//---- get the travel mode ----

		str_ptr = Get_Control_String (TRAVEL_MODE_CODE_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);

			if (lvalue < 0 || lvalue >= MAX_MODE) {
				Error ("Travel Mode %d is Out of Range (0..%d)", lvalue, MAX_MODE);
			}
			group->Mode (lvalue);
		} else {
			group->Mode (DRIVE_ALONE);
		}
		Print (1, "Travel Mode Code = %d", group->Mode ());

		//---- average speed ----

		str_ptr = Get_Control_String (AVERAGE_TRAVEL_SPEED_x, i);

		if (str_ptr != NULL) {
			Get_Double (str_ptr, &dvalue);

			if (dvalue < 1.0 || dvalue > 50.0) {
				Error ("Average Travel Speed %.1lf is Out of Range (1..50)", dvalue);
			}
		} else {
			dvalue = 10.0;
		}
		group->Speed (Round (dvalue));

		Print (1, "Average Travel Speed = %.1lf (meters/second)", UnRound (group->Speed ()));

		//---- get the vehicle type ----

		str_ptr = Get_Control_String (VEHICLE_TYPE_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);

			if (lvalue < 1 || lvalue > 20) {
				Error ("Vehicle Type %d is Out of Range (1..20)", lvalue);
			}
			group->Type (lvalue);
		} else {
			group->Type (1);
		}

		//---- get the vehicle subtype ----

		str_ptr = Get_Control_String (VEHICLE_SUBTYPE_x, i);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &lvalue);

			if (lvalue < 0 || lvalue > 20) {
				Error ("Vehicle Subtype %d is Out of Range (0..20)", lvalue);
			}
			group->SubType (lvalue);
		} else {
			group->SubType (0);
		}
		Print (1, "Vehicle Type = %d, Subtype = %d", group->Type (), group->SubType ());

		//---- get the trip adjustment factors ----

		str_ptr = Get_Control_String (TRIP_ADJUSTMENT_FACTORS_x, i);

		if (str_ptr != NULL) {
			group->Trip_Factor (new Factor_File ());

			str_fmt (buffer, sizeof (buffer), "Trip Adjustment Factors #%d", i);

			group->Trip_Factor ()->File_Type (buffer);

			format_ptr = Get_Control_String (TRIP_ADJUSTMENT_FORMAT_x, i);

			if (format_ptr != NULL) {
				group->Trip_Factor ()->Dbase_Format (format_ptr);
			}
			if (!group->Trip_Factor ()->Open (Project_Filename (str_ptr))) {
				Error ("Opening Trip Adjustment Factors #%d", i);
			}
			factor_flag = true;
		}

		//---- time period equivalence ----

		if (factor_flag || group->Trip_File ()->Period_Flag ()) {

			str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE_x, i);

			if (str_ptr != NULL) {
				if (!group->Time_Equiv_File (Project_Filename (str_ptr))) {
					Error ("Opening Time Period Equivalence #%d", i);
				}
			} else if (group->Trip_File ()->Period_Flag ()) {
				Warning ("Trip Table Time Periods will be Ignored");
			}
		}

		//---- set the field map ----

		if (group->Org_Wt () != 0) {
			j = field_map.Get_Index (group->Org_Wt ());

			if (j == 0) {
				field_map.Add (group->Org_Wt ());

				group->Org_Wt (field_map.Record_Index ());
			} else {
				group->Org_Wt (j);
			}
		}
		if (group->Des_Wt () != 0) {
			j = field_map.Get_Index (group->Des_Wt ());

			if (j == 0) {
				field_map.Add (group->Des_Wt ());

				group->Des_Wt (field_map.Record_Index ());
			} else {
				group->Des_Wt (j);
			}
		}

		//---- add the group record to the list ----

		file_group.Add ();
	}
	Write (2, "Highest Trip Group Number = %d", num_file);
	Write (1, "Number of Trip Table Groups = %d", file_group.Num_Records ());

	if (field_map.Num_Records () > 0) {
		Print (2, "Number of Unique Activity Location Weight Fields = %d", field_map.Num_Records ());

		Convert_Location *loc_ptr = (Convert_Location *) location_data.New_Record ();

		loc_ptr->Num_Weight (field_map.Num_Records ());
	}
	Show_Message (1);

	//---- read the zone equiv ----

	if (factor_flag) {
		if (equiv_flag) {
			zone_equiv.Read (Report_Flag (ZONE_EQUIV));
		} else {
			Write (2, "Trip Adjustment Factors are Zone-Based");
		}
	}

	//---- read the zone location map ----

	if (zone_loc_flag) {
		zone_loc_map.Read (false);
	}
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
