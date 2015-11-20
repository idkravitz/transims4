//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PopSyn.hpp"

#include "Utility.hpp"
#include "Location_File.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PopSyn::Program_Control (void)
{
	int i, j, k, num, field, attributes, types, location_field, persons_field;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];
	double dvalue;
	bool binary;

	Household_Model *model_ptr;
	Attribute_Type *at_ptr;
	Data_Range *data_breaks;
	PUMA_Index *index_ptr;

	Location_File *location_file;
	Db_Field *fld;	

	//---- open the pums household file ----

	str_ptr = Get_Control_String (PUMS_HOUSEHOLD_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	pums_hh_file.File_Type ("PUMS Household File");

	format_ptr = Get_Control_String (PUMS_HOUSEHOLD_FORMAT);

	if (format_ptr != NULL) {
		pums_hh_file.Dbase_Format (format_ptr);
	}
	pums_hh_file.Open (Project_Filename (str_ptr));

	//---- get required field names ----

	hh_state_field = pums_hh_file.Required_Field ("STATE");
	hh_puma_field = pums_hh_file.Required_Field ("PUMA");
	hh_hhold_field = pums_hh_file.Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	persons_field = pums_hh_file.Required_Field ("PERSONS", "HHSIZE", "SIZE");
	location_field = pums_hh_file.Optional_Field ("LOCATION");

	//---- open the pums population file ----

	str_ptr = Get_Control_String (PUMS_POPULATION_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	pums_pop_file.File_Type ("PUMS Population File");

	format_ptr = Get_Control_String (PUMS_POPULATION_FORMAT);

	if (format_ptr != NULL) {
		pums_pop_file.Dbase_Format (format_ptr);
	}
	pums_pop_file.Open (Project_Filename (str_ptr));

	//---- get required field names ----

	pop_hhold_field = pums_pop_file.Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	pop_person_field = pums_pop_file.Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");

	//---- open the zone data file ----

	str_ptr = Get_Control_String (ZONE_DATA_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	zone_file.File_Type ("Zone Data File");

	format_ptr = Get_Control_String (ZONE_DATA_FORMAT);

	if (format_ptr != NULL) {
		zone_file.Dbase_Format (format_ptr);
	}
	zone_file.Open (Project_Filename (str_ptr));

	//---- get required field names ----

	zone_state_field = zone_file.Required_Field ("STATE");
	zone_puma_field = zone_file.Required_Field ("PUMA");

	//---- open the nearest zone locations ----

	str_ptr = Get_Control_String (NEAREST_ZONE_LOCATION);

	if (str_ptr != NULL) {
		zone_loc_flag = true;

		Print (1);
		zone_loc_file.File_Type ("Nearest Zone Location");
		zone_loc_file.Open (Project_Filename (str_ptr));
	}

	//---- open the vehicle type distribution -----

	str_ptr = Get_Control_String (VEHICLE_TYPE_DISTRIBUTION);

	if (str_ptr != NULL) {
		veh_type_flag = true;

		Print (1);
		veh_type_file.File_Type ("Vehicle Type Distribution");
		veh_type_file.Open (Project_Filename (str_ptr));
	}

	//---- open network and demand files ----

	Demand_Service::Program_Control ();

	//---- open the problem file ----
	
	str_ptr = Get_Control_String (NEW_PROBLEM_FILE);

	if (str_ptr != NULL) {
		Print (1);

		format_ptr = Get_Control_String (NEW_PROBLEM_FORMAT);

		if (format_ptr != NULL) {
			problem_file.Dbase_Format (format_ptr);
		}
		problem_file.PopSyn_Flag ();
		problem_file.Create (Project_Filename (str_ptr, Extension ()));
		problem_flag = true;
	}

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
		random_type.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	//---- maximum IPF iterations ----

	str_ptr = Get_Control_String (MAXIMUM_IPF_ITERATIONS);

	if (str_ptr != NULL) {
		num = atoi (str_ptr);

		if (num < 100 || num > 100000) {
			Error ("Maximum IPF Iterations %d is Out of Range (100..100000)", num);
		}
		ipf_data.Max_Iterations (num);
		stage2_data.Max_Iterations (num);

		Print (1, "Maximum IPF Iterations = %d", num);
	}

	//---- maximum IPF difference ----

	str_ptr = Get_Control_String (MAXIMUM_IPF_DIFFERENCE);

	if (str_ptr != NULL) {
		dvalue = atof (str_ptr);

		if (dvalue <= 0.0 || dvalue > 0.001) {
			Error ("Maximum IPF Difference %0.9lf is Out of Range (>0.0..0.001)", dvalue);
		}
		ipf_data.Max_Difference (dvalue);
		stage2_data.Max_Difference (dvalue);

		Print (1, "Maximum IPF Difference = %0.9lf", dvalue);
	}
	
	//---- state puma list ----

	str_ptr = Get_Control_String (STATE_PUMA_LIST);

	if (str_ptr == NULL) goto control_error;

	Print (2, "State PUMA List = %s", str_ptr);

	while (str_ptr) {
		str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

		index_ptr = puma_index.PUMA_Key (buffer);
		if (index_ptr == NULL) goto puma_error;

		if (!puma_index.Add (index_ptr)) {
			Error ("Adding PUMA to the List");
		}
	}

	//---- pums weight field ----

	str_ptr = Get_Control_String (PUMS_WEIGHT_FIELD);

	if (str_ptr != NULL) {
		weight_field = pums_hh_file.Field_Number (str_ptr);

		if (weight_field == 0) {
			Error ("PUMS Weight Field %s was Not Found in the PUMS Household File", str_ptr);
		}
		Print (2, "PUMS Weight Field Name = %s, Number = %d", str_ptr, weight_field);
	} else {
		Print (2, "PUMS Records will Not be Weighted");
		weight_field = 0;
	}

	//---- pums vehicle field ----

	str_ptr = Get_Control_String (PUMS_VEHICLE_FIELD);

	if (str_ptr != NULL) {
		vehicle_field = pums_hh_file.Field_Number (str_ptr);

		if (vehicle_field == 0) {
			Error ("PUMS Vehicle Field %s was Not Found in the PUMS Household File", str_ptr);
		}
		Print (1, "PUMS Vehicle Field Name = %s, Number = %d", str_ptr, vehicle_field);
	} else {
		vehicle_field = 0;

		str_ptr = Get_Control_String (PUMS_AGE_FIELD);

		if (str_ptr != NULL) {
			age_field = pums_pop_file.Field_Number (str_ptr);

			if (age_field == 0) {
				Error ("PUMS Age Field %s was Not Found in the PUMS Population File", str_ptr);
			}
			Print (1, "PUMS Age Field Name = %s, Number = %d", str_ptr, age_field);
		} else {
			age_field = pums_pop_file.Field_Number ("AGE");
		}
		if (age_field > 0) {
			Print (1, "Vehicles will be Generated for Each Adult");
		} else {
			Print (1, "Vehicles will be Generated for All Household Members");
		}
	}

	//---- zone data id field ----

	str_ptr = Get_Control_String (ZONE_DATA_ID_FIELD);

	if (str_ptr != NULL) {
		zone_id_field = zone_file.Field_Number (str_ptr);

		if (zone_id_field == 0) {
			Error ("Zone ID Field %s was Not Found in the Zone Data File", str_ptr);
		}
		Print (1, "Zone Data ID Field Name = %s, Number = %d", str_ptr, zone_id_field);
	} else {
		zone_id_field = zone_file.Required_Field ("ZONE", "ZONE_ID", "ZONEID", "BG_ID", "ID");
	}

	//---- convert activity location fields ----

	location_file = (Location_File *) Network_Db_Base (ACTIVITY_LOCATION);
	
	//---- location zone field ----

	str_ptr = Get_Control_String (LOCATION_ZONE_FIELD);

	if (str_ptr == NULL) goto control_error;

	zone_field = location_file->Field_Number (str_ptr);

	if (zone_field == 0) {
		Error ("Zone Field %s was Not Found in the Activity Location File", str_ptr);
	}
	Print (1, "Location Zone Field Name = %s, Number = %d", str_ptr, zone_field);

	//---- get the number of household models ----

	num = Highest_Control_Group (PUMS_ATTRIBUTE_FIELD_x, 0);

	if (num == 0) {
		Error ("No Household Models were Found");
	}

	//---- process each household model ----

	for (i=1; i <= num; i++) {

		attributes = Highest_Control_Group (PUMS_ATTRIBUTE_FIELD_x, i, 0);

		if (attributes == 0) continue;
		if (attributes < 2) {
			Error ("Number of Attributes = %d for Model %d is Out of Range", attributes, i);
		}

		model_ptr = new Household_Model (i);

		Print (2, "Household Model #%d", i);

		//---- location weight field ----

		str_ptr = Get_Control_String (LOCATION_WEIGHT_FIELD_x, i);

		if (str_ptr != NULL) {

			field = location_file->Field_Number (str_ptr);

			if (field == 0) {
				Error ("Weight Field %s was Not Found in the Activity Location File", str_ptr);
			}
			Print (1, "Location Weight Field Name = %s, Number = %d", str_ptr, field);

			model_ptr->Location_Field (field);

		} else {
			model_ptr->Location_Field (0);

			Print (1, "Zone Locations will be Weighted Equally"); 
		}

		//---- zone total field ----

		str_ptr = Get_Control_String (ZONE_TOTAL_FIELD_x, i);

		if (str_ptr != NULL) {

			field = zone_file.Field_Number (str_ptr);

			if (field == 0) {
				Error ("Total Field %s was Not Found in the Zone Data File", str_ptr);
			}
			Print (1, "Zone Data Total Field Name = %s, Number = %d", str_ptr, field);

			model_ptr->Total_Field (field);

		} else {
			model_ptr->Total_Field (0);

			Print (1, "Zone Totals are based on Attribute Sums"); 
		}

		//---- process each attribute ----

		for (j=1; j <= attributes; j++) {

			str_ptr = Get_Control_String (PUMS_ATTRIBUTE_FIELD_x, i, j);

			if (str_ptr == NULL) goto control_error;

			at_ptr = model_ptr->New_Attribute ();

			field = pums_hh_file.Field_Number (str_ptr);

			if (field == 0) {
				Error ("PUMS Attribute Field %s was Not Found in the PUMS Household File", str_ptr);
			}
			Print (2, "PUMS Attribute Field #%d Name = %s, Number = %d", j, str_ptr, field);

			at_ptr->Field_Name (str_ptr);
			at_ptr->PUMS_Field (field);

			//---- get the data breaks ----

			str_ptr = Get_Control_String (PUMS_ATTRIBUTE_BREAKS_x, i, j);

			if (str_ptr == NULL) goto control_error;

			data_breaks = new Data_Range ();
			data_breaks->Add_Breaks (str_ptr);
			types = data_breaks->Num_Ranges ();

			if (types == 0) {
				Error ("PUMS Attribute Breaks %s as Illogical", str_ptr);
			}
			at_ptr->PUMS_Breaks (data_breaks);
			at_ptr->Num_Types (types);

			Print (1, "PUMS Attribute Breaks #%d = %s (%d Types)", j, str_ptr, types);

			//---- get the zone field group ----

			str_ptr = Get_Control_String (ZONE_FIELD_GROUP_x, i, j);

			if (str_ptr == NULL) goto control_error;

			Print (1, "Zone Field Group #%d = %s", j, str_ptr);

			//---- find each field ----

			for (k=1; k <= types; k++) {
				str_fmt (buffer, sizeof (buffer), "%s%d", str_ptr, k);

				field = zone_file.Field_Number (buffer);

				if (field == 0) {
					Error ("Zone Type Field %s was Not Found in the Zone File", buffer);
				}
				Print (1, "\tAttribute Type #%d Field = %s, Number = %d", k, buffer, field);

				at_ptr->Zone_Field (k, field);
			}
		}

		//---- set the record size ----

		if (!model_ptr->Set_Record ()) {
			Error ("Allocating Attribute Space");
		}

		//---- add pumas to the model ----

		for (index_ptr = puma_index.First (); index_ptr; index_ptr = puma_index.Next ()) {
			if (model_ptr->New_PUMA (puma_index.Record_Index ()) == NULL) {
				Error ("Adding PUMA Record");
			}
		}

		//---- add the model to the array ----

		if (!household_model.Add (model_ptr)) {
			Error ("Adding Household Model");
		}
	}

	//---- set the location weight size ----

	location_list.Num_Weight (household_model.Num_Records ());

	//---- starting household id ----

	str_ptr = Get_Control_String (STARTING_HOUSEHOLD_ID);

	if (str_ptr != NULL) {
		household = atoi (str_ptr);

		if (household < 1) {
			Error ("Starting Household ID %d is Out of Range", household);
		}
		Print (2, "Starting Household ID = %d", household);
	} else {
		Print (1);
	}

	//---- starting vehicle id ----

	str_ptr = Get_Control_String (STARTING_VEHICLE_ID);

	if (str_ptr != NULL) {
		vehicle = atoi (str_ptr);

		if (vehicle < 1) {
			Error ("Starting Vehicle ID %d is Out of Range", vehicle);
		}
		Print (1, "Starting Vehicle ID = %d", vehicle);
	}

	//---- output household file ----

	Print (1);
	household_db.File_Type ("Household Database");
	binary = (pums_hh_file.Record_Format () == BINARY);

	//---- add default fields ----

	household_db.Add_Field ("HHOLD", INTEGER, 10);
	household_db.Add_Field ("LOCATION", INTEGER, 10);

	//---- output household fields ----

	str_ptr = Get_Control_String (OUTPUT_HOUSEHOLD_FIELDS);

	if (str_ptr != NULL) {
		Print (1, "Output Household Fields = %s", str_ptr);

		//---- copy required fields ----

		fld = pums_hh_file.Field (persons_field);
		household_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);

		if (vehicle_field > 0) {
			fld = pums_hh_file.Field (vehicle_field);
			household_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}

		//---- copy selected fields ----

		while (str_ptr) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			field = pums_hh_file.Required_Field (buffer);
			if (field == hh_hhold_field || field == persons_field || field == location_field ||
				(vehicle_field > 0 && field == vehicle_field)) continue;

			fld = pums_hh_file.Field (field);

			household_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}

	} else {

		//---- copy all fields ----

		num = pums_hh_file.Num_Fields ();

		for (field=1; field <= num; field++) {
			if (field == hh_hhold_field || field == location_field) continue;

			fld = pums_hh_file.Field (field);
			if (fld == NULL) continue;

			household_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}
	}

	//---- update the output database fields ----

	new_hh_file = (Household_File *) Demand_Db_File (NEW_HOUSEHOLD);

	new_hh_file->Clear_Fields ();
	new_hh_file->Replicate_Fields (&household_db);
	new_hh_file->Write_Header ();

	//---- reset vehicle field number ----

	if (vehicle_field > 0) {
		str_ptr = Get_Control_String (PUMS_VEHICLE_FIELD);

		vehicle_field = household_db.Field_Number (str_ptr);
	}

	//---- output population file ----

	population_db.File_Type ("Population Database");
	binary = (pums_pop_file.Record_Format () == BINARY);

	//---- add default fields ----

	population_db.Add_Field ("HHOLD", INTEGER, 10);
	population_db.Add_Field ("PERSON", INTEGER, 2);

	//---- output population fields ----

	str_ptr = Get_Control_String (OUTPUT_POPULATION_FIELDS);

	if (str_ptr != NULL) {
		Print (1, "Output Population Fields = %s", str_ptr);

		//---- copy required fields ----

		if (age_field > 0) {
			fld = pums_pop_file.Field (age_field);
			population_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}

		//---- copy selected fields ----

		while (str_ptr) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

			field = pums_pop_file.Required_Field (buffer);
			if (field == pop_hhold_field || field == pop_person_field ||
				(age_field > 0 && field == age_field)) continue;

			fld = pums_pop_file.Field (field);

			population_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}

	} else {

		//---- copy all fields ----

		num = pums_pop_file.Num_Fields ();

		for (i=1; i <= num; i++) {
			if (i == pop_hhold_field || i == pop_person_field) continue;

			fld = pums_pop_file.Field (i);
			if (fld == NULL) continue;

			population_db.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
		}
	}

	//---- update the output database fields ----

	new_pop_file = (Population_File *) Demand_Db_File (NEW_POPULATION);

	new_pop_file->Clear_Fields ();
	new_pop_file->Replicate_Fields (&population_db);
	new_pop_file->Write_Header ();

	//---- reset age field number ----

	if (age_field > 0) {
		str_ptr = Get_Control_String (PUMS_AGE_FIELD);

		if (str_ptr != NULL) {
			age_field = population_db.Field_Number (str_ptr);
		} else {
			age_field = population_db.Field_Number ("AGE");
		}
	}

	//---- get the vehicle file pointer ----

	new_veh_file = (Vehicle_File *) Demand_Db_File (NEW_VEHICLE);

	//---- get the warning flag ----

	str_ptr = Get_Control_String (LOCATION_WARNING_FLAG);

	if (str_ptr != NULL) {
		Print (2, "Location Warning Flag = %s", str_ptr);

		warning_flag = Get_Control_Flag (LOCATION_WARNING_FLAG);
	}

	//---- read report types ----

	List_Reports ();

	puma_report_flag = Report_Flag (CROSS_CLASS);
	hh_sum_flag = Report_Flag (SYN_HOUSEHOLD);
	pop_sum_flag = Report_Flag (SYN_POPULATION);

	//---- initialize the household data -----

	if (hh_sum_flag) {
		hh_fields = new_hh_file->Num_Fields () + 1;

		hh_total = new double [hh_fields];
		hh_minimum = new double [hh_fields];
		hh_maximum = new double [hh_fields];

		if (hh_total == NULL || hh_minimum == NULL || hh_maximum == NULL) goto mem_error;

		memset (hh_total, '\0', hh_fields * sizeof (double));
		memset (hh_maximum, '\0', hh_fields * sizeof (double));

		for (i=1; i <= hh_fields; i++) {
			hh_minimum [i] = MAX_INTEGER;
		}
	}

	//---- initialize the population data ----

	if (pop_sum_flag) {
		pop_fields = new_pop_file->Num_Fields () + 1;

		pop_total = new double [pop_fields];
		pop_minimum = new double [pop_fields];
		pop_maximum = new double [pop_fields];

		if (pop_total == NULL || pop_minimum == NULL || pop_maximum == NULL) goto mem_error;

		memset (pop_total, '\0', pop_fields * sizeof (double));
		memset (pop_maximum, '\0', pop_fields * sizeof (double));

		for (i=1; i <= pop_fields; i++) {
			pop_minimum [i] = MAX_INTEGER;
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for Summary Data");

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

puma_error:
	Error ("PUMA Code %s is Incorrect", buffer);
} 
