//*********************************************************
//	Read_Scripts.cpp - read the user program scripts
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Read_Scripts
//---------------------------------------------------------

void ModeChoice::Read_Scripts (void)
{
	int i, num_db_files;

	Model_Group *model_ptr;
	Skim_Group *skim_ptr;
	Mode_Group *mode_ptr;
	Db_Base **db_files;

	Print (1);

	//---- create the trip info record ----

	trip_info.Initialize (MODIFY, BINARY);
	trip_info.File_ID ("Trip");

	trip_info.Add_Field ("NUMBER", INTEGER, 6);
	trip_info.Add_Field ("MODE", INTEGER, 2);
	trip_info.Add_Field ("PURPOSE", INTEGER, 2);
	trip_info.Add_Field ("START_TIME", INTEGER, 6);
	trip_info.Add_Field ("END_TIME", INTEGER, 6);
	trip_info.Add_Field ("DURATION", INTEGER, 6);
	trip_info.Add_Field ("DISTANCE", INTEGER, 6);
	trip_info.Add_Field ("TTIME", INTEGER, 6);

	trip_number = trip_info.Field_Number ("NUMBER");
	trip_mode = trip_info.Field_Number ("MODE");
	trip_purpose = trip_info.Field_Number ("PURPOSE");
	trip_start = trip_info.Field_Number ("START_TIME");
	trip_end = trip_info.Field_Number ("END_TIME");
	trip_duration = trip_info.Field_Number ("DURATION");
	trip_distance = trip_info.Field_Number ("DISTANCE");
	trip_ttime = trip_info.Field_Number ("TTIME");

	if (method > 0) {

		//---- create the tour info record ----

		tour_info.Initialize (MODIFY, BINARY);
		tour_info.File_ID ("Tour");

		tour_info.Add_Field ("FIRST_TRIP", INTEGER, 2);
		tour_info.Add_Field ("ARRIVE_TRIP", INTEGER, 2);
		tour_info.Add_Field ("RETURN_TRIP", INTEGER, 2);
		tour_info.Add_Field ("LAST_TRIP", INTEGER, 2);
		tour_info.Add_Field ("MODE", INTEGER, 2);
		tour_info.Add_Field ("PURPOSE", INTEGER, 2);
		tour_info.Add_Field ("START_TIME", INTEGER, 6);
		tour_info.Add_Field ("END_TIME", INTEGER, 6);
		tour_info.Add_Field ("DURATION", INTEGER, 6);
		tour_info.Add_Field ("STOPS1", INTEGER, 2);
		tour_info.Add_Field ("STOPS2", INTEGER, 2);
		tour_info.Add_Field ("SUBTOURS", INTEGER, 2);
		tour_info.Add_Field ("DISTANCE1", INTEGER, 6);
		tour_info.Add_Field ("DISTANCE2", INTEGER, 6);
		tour_info.Add_Field ("TTIME1", INTEGER, 6);
		tour_info.Add_Field ("TTIME2", INTEGER, 6);

		tour_first = tour_info.Field_Number ("FIRST_TRIP");
		tour_arrive = tour_info.Field_Number ("ARRIVE_TRIP");
		tour_return = tour_info.Field_Number ("RETURN_TRIP");
		tour_last = tour_info.Field_Number ("LAST_TRIP");
		tour_mode = tour_info.Field_Number ("MODE");
		tour_purpose = tour_info.Field_Number ("PURPOSE");
		tour_start = tour_info.Field_Number ("START_TIME");
		tour_end = tour_info.Field_Number ("END_TIME");
		tour_duration = tour_info.Field_Number ("DURATION");
		tour_stops1 = tour_info.Field_Number ("STOPS1");
		tour_stops2 = tour_info.Field_Number ("STOPS2");
		tour_subtours = tour_info.Field_Number ("SUBTOURS");
		tour_distance1 = tour_info.Field_Number ("DISTANCE1");
		tour_distance2 = tour_info.Field_Number ("DISTANCE2");
		tour_ttime1 = tour_info.Field_Number ("TTIME1");
		tour_ttime2 = tour_info.Field_Number ("TTIME2");
	}
	
	//---- create the database record array ----

	num_db_files = 4 + skim_group.Num_Records () + mode_group.Num_Records ();

	if (method > 0) {
		num_db_files++;
	}
	if (person_flag) {
		num_db_files++;
	}
	if (zone_flag) {
		num_db_files += 2;
	}
	db_files = new Db_Base * [num_db_files];
	if (db_files == NULL) {
		Error ("Insufficient Space for %d Database Files", num_db_files);
	}
	i = 0;

	db_files [i++] = &trip_info;
	if (method > 0) {
		db_files [i++] = &tour_info;
	}
	db_files [i++] = &household_db;
	if (person_flag) {
		db_files [i++] = &person_db;
	}
	db_files [i++] = &org_loc;
	db_files [i++] = &des_loc;
	if (zone_flag) {
		db_files [i++] = &org_zone;
		db_files [i++] = &des_zone;
	}
	for (skim_ptr = skim_group.First (); skim_ptr; skim_ptr = skim_group.Next ()) {
		db_files [i++] = skim_ptr->Skim ();
	}
	for (mode_ptr = mode_group.First (); mode_ptr; mode_ptr = mode_group.Next ()) {
		db_files [i++] = mode_ptr->Data ();
	}

	//---- compile each trip mode model ----

	for (model_ptr = trip_group.First (); model_ptr; model_ptr = trip_group.Next ()) {

		report_code = model_ptr->Group ();

		Write (1, "Compiling Trip Mode Model #%d", report_code);

		if (Report_Flag (TRIP_SCRIPT)) {
			Header_Number (TRIP_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}

		if (!model_ptr->Compile (num_db_files, db_files, Report_Flag (TRIP_SCRIPT))) {
			Error ("Compiling Trip Mode Model #%d", report_code);
		}
		if (Report_Flag (TRIP_STACK)) {
			Header_Number (TRIP_STACK);

			model_ptr->Stack_Report (false);
		}
		Header_Number (0);
	}

	//---- compile each tour mode model ----

	if (method > 0) {
		for (model_ptr = tour_group.First (); model_ptr; model_ptr = tour_group.Next ()) {

			report_code = model_ptr->Group ();

			Write (1, "Compiling Tour Mode Model #%d", report_code);

			if (Report_Flag (TOUR_SCRIPT)) {
				Header_Number (TOUR_SCRIPT);

				if (!Break_Check (10)) {
					Print (1);
					Page_Header ();
				}
			}

			if (!model_ptr->Compile (num_db_files, db_files, Report_Flag (TOUR_SCRIPT))) {
				Error ("Compiling Tour Mode Model #%d", report_code);
			}
			if (Report_Flag (TOUR_STACK)) {
				Header_Number (TOUR_STACK);

				model_ptr->Stack_Report (false);
			}
			Header_Number (0);
		}
	}
	Show_Message (1);

	delete [] db_files;	
}
