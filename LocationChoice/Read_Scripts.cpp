//*********************************************************
//	Read_Scripts.cpp - read the user program scripts
//*********************************************************

#include "LocationChoice.hpp"

//---------------------------------------------------------
//	Read_Scripts
//---------------------------------------------------------

void LocationChoice::Read_Scripts (void)
{
	Print (1);

	//---- create the tour info record ----

	tour_info.Initialize (Db_Code::MODIFY, Db_Code::BINARY);
	tour_info.File_ID ("Tour");

	tour_info.Add_Field ("START_TIME", Db_Code::INTEGER, 6);
	tour_info.Add_Field ("END_TIME", Db_Code::INTEGER, 6);
	tour_info.Add_Field ("DURATION", Db_Code::INTEGER, 6);
	tour_info.Add_Field ("PURPOSE", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("MODE", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("STOPS1", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("STOPS2", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("SUBTOURS", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("DISTANCE1", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("DISTANCE2", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("BUDGET1", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("BUDGET2", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("UTILITY", Db_Code::DOUBLE, 16, 8);
	tour_info.Add_Field ("MIN_FAC1", Db_Code::DOUBLE, 8, 4);
	tour_info.Add_Field ("MAX_FAC1", Db_Code::DOUBLE, 8, 4);
	tour_info.Add_Field ("MIN_FAC2", Db_Code::DOUBLE, 8, 4);
	tour_info.Add_Field ("MAX_FAC2", Db_Code::DOUBLE, 8, 4);
	tour_info.Add_Field ("ZONE1", Db_Code::INTEGER, 6);
	tour_info.Add_Field ("ZONE2", Db_Code::INTEGER, 6);
	tour_info.Add_Field ("LOCATION1", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("LOCATION2", Db_Code::INTEGER, 10);
	tour_info.Add_Field ("PURPOSE1", Db_Code::INTEGER, 2);
	tour_info.Add_Field ("PURPOSE2", Db_Code::INTEGER, 2);

	start_field = tour_info.Field_Number ("START_TIME");
	end_field = tour_info.Field_Number ("END_TIME");
	duration_field = tour_info.Field_Number ("DURATION");
	purpose_field = tour_info.Field_Number ("PURPOSE");
	mode_field = tour_info.Field_Number ("MODE");
	stops1_field = tour_info.Field_Number ("STOPS1");
	stops2_field = tour_info.Field_Number ("STOPS2");
	subtours_field = tour_info.Field_Number ("SUBTOURS");
	distance1_field = tour_info.Field_Number ("DISTANCE1");
	distance2_field = tour_info.Field_Number ("DISTANCE2");
	budget1_field = tour_info.Field_Number ("BUDGET1");
	budget2_field = tour_info.Field_Number ("BUDGET2");
	utility_field = tour_info.Field_Number ("UTILITY");
	min_fac1_field = tour_info.Field_Number ("MIN_FAC1");
	max_fac1_field = tour_info.Field_Number ("MAX_FAC1");
	min_fac2_field = tour_info.Field_Number ("MIN_FAC2");
	max_fac2_field = tour_info.Field_Number ("MAX_FAC2");
	zone1_field = tour_info.Field_Number ("ZONE1");
	zone2_field = tour_info.Field_Number ("ZONE2");
	loc1_field = tour_info.Field_Number ("LOCATION1");
	loc2_field = tour_info.Field_Number ("LOCATION2");
	purp1_field = tour_info.Field_Number ("PURPOSE1");
	purp2_field = tour_info.Field_Number ("PURPOSE2");

	//---- create the database record array ----

	int i, j;

	num_db_files = 2 + hhold_flag + 2 * skim_group.Num_Records ();
	db_files = new Db_Base * [num_db_files];

	j=0;

	db_files [j++] = &tour_info;
	db_files [j++] = &location_db;

	if (hhold_flag) {
		db_files [j++] = &household_db;
	}

	for (i=1; i <= skim_group.Num_Records (); i++) {
		db_files [j++] = skim_group [i]->Skim_A ();
		db_files [j++] = skim_group [i]->Skim_B ();
	}

	//---- compile each script ----

	Purpose_Group *group_ptr;

	for (group_ptr = purpose_group.First (); group_ptr; group_ptr = purpose_group.Next ()) {

		if (group_ptr->Model ()) {
			purpose_number = group_ptr->Group ();

			Write (1, "Compiling Location Choice Script #%d", purpose_number);

			if (Report_Flag (LOCATION_SCRIPT)) {
				Header_Number (LOCATION_SCRIPT);

				if (!Break_Check (10)) {
					Print (1);
					Page_Header ();
				}
			}
			if (group_ptr->Method ()) {
				db_files [1] = &zone_db;
			} else {
				db_files [1] = &location_db;
			}
			if (!group_ptr->Compile (num_db_files, db_files, Report_Flag (LOCATION_SCRIPT))) {
				Error ("Compiling Location Choice Script");
			}
			if (Report_Flag (LOCATION_STACK)) {
				Header_Number (LOCATION_STACK);

				group_ptr->Stack_Report (false);
			}
			Header_Number (0);
		}
	}
	Show_Message (1);
}
