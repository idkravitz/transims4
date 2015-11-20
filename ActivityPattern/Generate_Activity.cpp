//*********************************************************
//	Generate_Activity.cpp - generate household activity
//*********************************************************

#include "ActivityPattern.hpp"

//---------------------------------------------------------
//	Generate_Activity
//---------------------------------------------------------

void ActivityPattern::Generate_Activity (void)
{
	int hh, last_hh;
	bool stat;

	HHold_Data *household_ptr;
	
	//---- household match report ----

	if (Report_Flag (HOUSEHOLD_MATCH)) {
		Header_Number (HOUSEHOLD_MATCH);

		if (!Break_Check (population_data.Num_Records () + 4)) {
			Print (1);
			Page_Header ();
		}
	}
	Show_Message ("Generating Activities -- Household");
	Set_Progress (10);

	//---- get the first records ----

	if (update_flag) {
		stat = pattern_file.Read ();
		last_hh = 0;
	} else {
		household_ptr = household_data.First_Key ();
		stat = (household_ptr != NULL);
	}

	//---- process each record ---

	while (stat) {
		Show_Progress ();

		if (update_flag) {
			hh = pattern_file.Household ();

			//---- skip records from the last household ----

			if (hh == last_hh) {
				stat = pattern_file.Read ();
				continue;
			}

			//---- check the household list ----

			household_ptr = household_data.Get (hh);

			if (household_ptr == NULL) {
				new_pattern_file.Copy_Fields (&pattern_file);

				//---- reformat the time strings -----

				int start, end;

				start = activity_time.Step (pattern_file.Start_Min ());
				end = activity_time.Step (pattern_file.End_Min ());

				new_pattern_file.Start_Min (activity_time.Format_Step (start));
				new_pattern_file.End_Min (activity_time.Format_Step (end));
				new_pattern_file.Time_Min (activity_time.Format_Duration (end - start));

				start = activity_time.Step (pattern_file.Start_Max ());
				end = activity_time.Step (pattern_file.End_Max ());

				new_pattern_file.Start_Max (activity_time.Format_Step (start));
				new_pattern_file.End_Max (activity_time.Format_Step (end));
				new_pattern_file.Time_Max (activity_time.Format_Duration (end - start));

				if (!new_pattern_file.Write ()) {
					Error ("Writing Activity File");
				}
				stat = pattern_file.Read ();
				continue;
			}
			last_hh = hh;
		}

		//---- match the household ----

		Household_Match (household_ptr);

		//---- assign vehicles to drivers ----

		Assign_Vehicles ();

		//---- write the activities ----

		Write_Pattern ();

		//---- get the next record ----

		if (update_flag) {
			stat = pattern_file.Read ();
		} else {
			household_ptr = household_data.Next_Key ();
			stat = (household_ptr != NULL);
		}
	}
	End_Progress ();

	Header_Number (0);
	Break_Check (20);

	Print (2, "Number of Households = %d", nhhold);
	Print (1, "Number of Persons = %d", nperson);
	Print (1, "Number of Workers = %d", nwork);

	if (nhhold == 0) nhhold = 1;

	Print (2, "Persons per Household = %.2lf", (double) nperson / nhhold);
	Print (1, "Workers per Household = %.2lf", (double) nwork / nhhold);

	if (nperson == 0) nperson = 1;

	if (hh_match || per_match) Print (1);

	if (hh_match) {
		Print (1, "Number of Households with Match Problems = %d (%.1lf%%)", 
			hh_match, (100.0 * hh_match / nhhold));
	}
	if (per_match) {
		Print (1, "Number of Persons with Match Problems = %d (%.1lf%%)", 
			per_match, (100.0 * per_match / nperson));
	}

	nhhold -= hh_match;
	if (nhhold < 1) nhhold = 1;

	nperson -= per_match;
	if (nperson < 1) nperson = 1;

	if (age_match || work_match || gender_match) Print (1);

	if (age_match) {
		Print (1, "Number of Persons matched to a different Age Group = %d (%.1lf%%)", 
			age_match, (100.0 * age_match / nperson));
	}
	if (work_match) {
		Print (1, "Number of Persons matched to a different Work Status = %d (%.1lf%%)", 
			work_match, (100.0 * work_match / nperson));
	}
	if (gender_match) {
		Print (1, "Number of Persons matched to a different Gender = %d (%.1lf%%)", 
			gender_match, (100.0 * gender_match / nperson));
	}

	if (num_problem > 0 || num_act_gen != num_activity) {
		Print (2, "Number of Activities Generated = %d", num_act_gen);

		if (num_problem > 0) {
			Print (1, "Number of Activities with Problems = %d (%.1lf%%)", num_problem,
				(100.0 * num_problem / num_act_gen));
		}
		Print (1, "Number of Activities Written = %d (%.1lf%%)", num_activity,
			(100.0 * num_activity / num_act_gen));
	} else {
		Print (2, "Number of Activities = %d", num_activity);
	}
	Print (2, "Activities per Household = %.2lf", (double) num_activity / nhhold);
	Print (1, "Activities per Person = %.2lf", (double) num_activity / nperson);
}

