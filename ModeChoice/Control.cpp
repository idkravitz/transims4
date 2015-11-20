//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ModeChoice.hpp"

#include "Activity_File.hpp"
#include "Trip_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ModeChoice::Program_Control (void)
{
	int i, j, k, num, fields, value, type, size, decimal;
	char *str_ptr, *format_ptr, buffer [STRING_BUFFER], name [FIELD_BUFFER];
	double factor;
	
	Model_Group *model_ptr;
	Skim_Group *skim_ptr;
	Mode_Group *mode_ptr;

	char *methods [] = { "TRIP", "TRIP-TOUR", "TRIP-TOUR-TRIP", NULL };

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- set processing flags ----
	
	zone_flag = Network_File_Flag (ZONE);
	person_flag = Demand_File_Flag (POPULATION);
	activity_flag = Demand_File_Flag (ACTIVITY);
	trip_flag = Demand_File_Flag (TRIP);

	if (!activity_flag && !trip_flag) {
		Error ("A Trip and/or Activity File is Required");
	}
	if (activity_flag) {
		if (Demand_File_Flag (NEW_ACTIVITY)) {
			new_act_flag = true;

			Activity_File *file, *new_file;

			file = (Activity_File *) Demand_Db_Base (ACTIVITY);
			new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);

			new_file->Write_Header ();
		} else {
			Warning ("Activity Modes will Not be Saved");
			Show_Message (1);
		}
		Demand_File_False (ACTIVITY);
	} else {
		Demand_File_False (NEW_ACTIVITY);
	}
	if (trip_flag) {
		if (Demand_File_Flag (NEW_TRIP)) {
			new_trip_flag = true;

			Trip_File *file, *new_file;

			file = (Trip_File *) Demand_Db_Base (TRIP);
			new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);

			new_file->Write_Header ();
		} else {
			Warning ("Trip Modes will Not be Saved");
			Show_Message (1);
		}
		Demand_File_False (TRIP);
	} else {
		Demand_File_False (NEW_TRIP);
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

		if (hhlist_file.Extend ()) {
			if (activity_flag) {
				Activity_File *file = (Activity_File *) Demand_Db_Base (ACTIVITY);

				if (!file->Extend ()) {
					Warning ("Partitioned Household Lists are Illogical");
				}
			}
			if (trip_flag) {
				Trip_File *file = (Trip_File *) Demand_Db_Base (TRIP);

				if (!file->Extend ()) {
					Warning ("Partitioned Household Lists are Illogical");
				}
			}
		}
	}

	//---- model processing method ----
	
	str_ptr = Get_Control_String (MODEL_PROCESSING_METHOD);

	if (str_ptr != NULL) {
		for (i=0; methods [i] != NULL; i++) {
			if (str_cmp (str_ptr, methods [i]) == 0) {
				method = i;
				break;
			}
		}
		if (method != i) {
			Error ("Model Processing Method %s was Not Recognized", str_ptr);
		}
	}
	Print (2, "Model Processing Method = %s", methods [method]);

	if (method > 0) {

		//---- tour purpose hierarchy ----
	
		str_ptr = Get_Control_String (TOUR_PURPOSE_HIERARCHY);

		if (str_ptr == NULL) goto control_error;

		Print (2, "Tour Purpose Hierarchy = %s", str_ptr);

		i = 0;

		while (str_ptr && i < MAX_PURPOSE) {
			str_ptr = Get_Integer (str_ptr, &value);

			if (value > 0 && value < MAX_PURPOSE) {
				purpose_order [i++] = value;
			}
		}

		//---- get the number of tour model groups ----

		num = Highest_Control_Group (TOUR_MODE_MODEL_x, 0);

		if (num == 0) {
			Error ("No Tour Mode Models were Found");
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {
			
			//---- get the tour mode model -----

			str_ptr = Get_Control_String (TOUR_MODE_MODEL_x, i);

			if (str_ptr == NULL) continue;

			model_ptr = new Model_Group (i);

			Print (1);

			model_ptr->Script_File ("Tour", Project_Filename (str_ptr));

			if (!tour_group.Add (model_ptr)) {
				Error ("Adding Tour Mode Model");
			}

			//---- get the tour purpose range ----

			str_ptr = Get_Control_String (TOUR_PURPOSE_RANGE_x, i);

			if (str_ptr == NULL) continue;

			Print (1, "Tour Purpose Range #%d = %s", i, str_ptr);

			if (!tour_group.Purpose_Range (str_ptr)) {
				Error ("Tour Purpose Range #%d", i);
			}
		}
	}

	//---- get the number of trip model groups ----

	num = Highest_Control_Group (TRIP_MODE_MODEL_x, 0);

	if (num == 0) {
		Error ("No Trip Mode Models were Found");
	}

	//---- process each file ----

	for (i=1; i <= num; i++) {
		
		//---- get the trip mode model -----

		str_ptr = Get_Control_String (TRIP_MODE_MODEL_x, i);

		if (str_ptr == NULL) continue;

		model_ptr = new Model_Group (i);

		Print (1);

		model_ptr->Script_File ("Trip", Project_Filename (str_ptr));

		if (!trip_group.Add (model_ptr)) {
			Error ("Adding Trip Mode Model");
		}

		//---- get the trip purpose range ----

		str_ptr = Get_Control_String (TRIP_PURPOSE_RANGE_x, i);

		if (str_ptr == NULL) continue;

		Print (1, "Trip Purpose Range #%d = %s", i, str_ptr);

		if (!trip_group.Purpose_Range (str_ptr)) {
			Error ("Trip Purpose Range #%d", i);
		}
	}

	//---- get the number of modes ----

	num = Highest_Control_Group (MODE_DATA_MODE_x, 0);

	if (num == 0) {
		Warning ("No Mode Data were Found");
	} else {
		mode_flag = true;
	}

	//---- process each file ----

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (MODE_DATA_MODE_x, i);

		if (str_ptr == NULL) continue;

		Print (2, "Mode Data Mode #%d = %s", i, str_ptr);

		//---- create the mode data group ----

		mode_ptr = new Mode_Group (i);
		mode_ptr->Mode (atoi (str_ptr));

		if (!mode_group.Add (mode_ptr)) {
			Error ("Adding Mode Data Group");
		}

		//---- get the mode label ----

		str_ptr = Get_Control_String (MODE_DATA_LABEL_x, i);

		if (str_ptr != NULL) {

			Get_Token (str_ptr, name, sizeof (name));

			Print (1, "Mode Data Label #%d = %s", i, name);

			if (str_cmp (str_ptr, name) != 0) {
				Warning ("Mode Data Label %s is Not a Single String", str_ptr);
			}
			mode_ptr->Label (name);
		}

		//---- get the mode data fields ----

		fields = Highest_Control_Group (MODE_DATA_FIELD_x, i, 0);

		if (fields == 0) {
			Error ("Mode Data #%d requires Mode Data Fields", i);
		}

		for (j=1; j <= fields; j++) {
			str_ptr = Get_Control_String (MODE_DATA_FIELD_x, i, j);

			if (str_ptr == NULL) continue;

			Print (1, "Mode Data Field #%d-%d = %s", i, j, str_ptr);

			str_ptr = Get_Token (str_ptr, name, sizeof (name));
			if (name [0] == '\0') goto data_error;

			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0' || buffer [0] == 'I') {
				type = INTEGER;
			} else if (buffer [0] == 'R' || buffer [0] == 'D') {
				type = DOUBLE;
			} else if (buffer [0] == 'S' || buffer [0] == 'C') {
				type = STRING;
			} else {
				goto data_error;
			}
			str_ptr = Get_Token (str_ptr, buffer);
			if (buffer [0] == '\0') {
				size = 10;
				if (type == DOUBLE) {
					decimal = 2;
				} else {
					decimal = 0;
				}
			} else {
				size = atoi (buffer);

				if (type == DOUBLE) {
					str_ptr = strchr (buffer, '.');
					if (str_ptr != NULL) {
						decimal = atoi (str_ptr+1);
					} else {
						decimal = 2;
					}
				} else {
					decimal = 0;
				}
			}
			if (!mode_ptr->Add_Field (name, (Field_Type) type, size, decimal)) {
				Error ("Adding Mode Data Field \"%s\"", name);
			}
		}	
	}

	//---- get the number of skims ----

	num = Highest_Control_Group (ZONE_SKIM_FILE_x, 0);

	if (num != 0) {
		skim_flag = true;

		//---- process each file ----

		for (i=1; i <= num; i++) {
			
			str_ptr = Get_Control_String (ZONE_SKIM_FILE_x, i);

			if (str_ptr == NULL) continue;

			if (!Break_Check (4)) {
				Print (1);
			}
			skim_ptr = new Skim_Group ();

			skim_ptr->Group (i);

			format_ptr = Get_Control_String (ZONE_SKIM_FORMAT_x, i);

			skim_ptr->Skim_Filename (Project_Filename (str_ptr), format_ptr);
			
			//---- time period equiv ----

			str_ptr = Get_Control_String (TIME_PERIOD_EQUIVALENCE_x, i);

			if (str_ptr != NULL) {
				skim_ptr->Time_Period_File (Project_Filename (str_ptr));
			}

			//---- skim memory factor ----

			str_ptr = Get_Control_String (SKIM_MEMORY_FACTOR_x, i);

			if (str_ptr != NULL) {
				factor = atof (str_ptr);

				if (factor < 0.05 || factor > 1.0) {
					Error ("Skim Memory Factor %.2lf is Out of Range (0.05..1.0)", factor);
				}
				skim_ptr->Memory_Factor (factor);
				Print (1, "Skim Memory Factor #%d = %.2lf", i, factor);
			}

			//---- save the skim group ----

			if (!skim_group.Add (skim_ptr)) {
				Error ("Adding Zone Skim Group");
			}
		}
	}

	//---- get the distance calculation ----

	str_ptr = Get_Control_String (DISTANCE_CALCULATION);

	if (str_ptr != NULL) {
		distance_method = Distance_Code (str_ptr);
	}
	Print (2, "Distance Calculation = %s", Distance_Code (distance_method));

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		value = atoi (str_ptr);
	} else {
		value = 0;
	}

	//---- apply the random seed to each model ----

	model_ptr = trip_group.First ();

	value = model_ptr->Random_Seed (value);

	Print (2, "Random Number Seed = %d", value);

	k = 100;

	while ((model_ptr = trip_group.Next ())) {
		k--;
		value = ((value / k) + (value % k)) * (200 - k);
		model_ptr->Random_Seed (value);
	}

	for (model_ptr = tour_group.First (); model_ptr; model_ptr = tour_group.Next ()) {
		k--;
		value = ((value / k) + (value % k)) * (200 - k);
		model_ptr->Random_Seed (value);
	}

	//---- get the mode share details file ----

	str_ptr = Get_Control_String (MODE_SHARE_DETAILS);

	if (str_ptr != NULL) {
		share_flag = true;
		share_file.File_Type ("Mode Share Details");
		share_file.File_Access (CREATE);
		Print (1);

		format_ptr = Get_Control_String (MODE_SHARE_FORMAT);

		if (format_ptr != NULL) {
			share_file.Dbase_Format (format_ptr);
		} else {
			share_file.Dbase_Format (DEFAULT_FORMAT);
		}
		if (!share_file.Open (Project_Filename (str_ptr))) {
			Error ("Opening %s", share_file.File_Type ());
		}
		share_file.Add_Field ("HOUSEHOLD", INTEGER, 10);
		share_file.Add_Field ("PERSON", INTEGER, 5);
		share_file.Add_Field ("TRIP", INTEGER, 5);
		share_file.Add_Field ("PURPOSE", INTEGER, 3);
		share_file.Add_Field ("START", STRING, TIME_BUFFER);
		share_file.Add_Field ("ORIGIN", INTEGER, 10);
		share_file.Add_Field ("ORG_ZONE", INTEGER, 6);
		share_file.Add_Field ("ARRIVE", STRING, TIME_BUFFER);
		share_file.Add_Field ("DESTINATION", INTEGER, 10);
		share_file.Add_Field ("DES_ZONE", INTEGER, 6);
		share_file.Add_Field ("CHOICE", STRING, 20);

		if (mode_flag) {
			Mode_Group *mode_ptr;

			for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
				share_file.Add_Field (mode_ptr->Label (), DOUBLE, 10, 6);
			}
		}
		share_file.Write_Header ();
	}
	detail_flag = Report_Flag (SHARE_DETAILS);

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "24_HOUR_CLOCK";
	}
	if (!time_step.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- set the database fields ----

	household_db.Replicate_Fields (Demand_Db_Base (HOUSEHOLD), false);
	household_db.File_ID ("HHold");
	household_db.Data_Size (household_db.Max_Size ());

	if (person_flag) {
		person_db.Add_Field ("KEY", INTEGER, sizeof (int), 0, 0, true);
		person_db.Replicate_Fields (Demand_Db_Base (POPULATION), false);
		person_db.File_ID ("Person");
		person_db.Data_Size (person_db.Max_Size ());
	}
	location_db.Replicate_Fields (Network_Db_Base (ACTIVITY_LOCATION), false);
	location_db.Data_Size (location_db.Max_Size ());

	org_loc.File_Access (MODIFY);
	org_loc.Dbase_Format (BINARY);
	org_loc.File_ID ("From");
	org_loc.Replicate_Fields (&location_db);

	des_loc.File_Access (MODIFY);
	des_loc.Dbase_Format (BINARY);
	des_loc.File_ID ("To");
	des_loc.Replicate_Fields (&location_db);

	if (zone_flag) {
		zone_db.Replicate_Fields (Network_Db_Base (ZONE), false);
		zone_db.Data_Size (zone_db.Max_Size ());

		org_zone.File_Access (MODIFY);
		org_zone.Dbase_Format (BINARY);
		org_zone.File_ID ("Org");
		org_zone.Replicate_Fields (&zone_db);

		des_zone.File_Access (MODIFY);
		des_zone.Dbase_Format (BINARY);
		des_zone.File_ID ("Des");
		des_zone.Replicate_Fields (&zone_db);
	}
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());

data_error:
	Error ("Mode Data Field #%d-%d is Improperly Specified", i, j);
}
