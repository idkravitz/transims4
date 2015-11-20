//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "ModeChoice.hpp"

#include "Activity_File.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

bool ModeChoice::Read_Activity (int file_num)
{
	int hhold, person, time1, time2, prev_purp, purpose, destination, new_mode;
	bool stat;

	Activity_File *activity_file, *new_file;
	Population_Data pop_rec;
	Travel_Leg leg, tour;

	activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);

	if (file_num > 0) {
		if (!activity_file->Open (file_num)) return (false);

		if (new_act_flag) {
			new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

			if (!new_file->Open (file_num)) {
				Error ("Creating %s", new_file->File_Type ());
			}
			new_file->Clear_Fields ();
			new_file->Replicate_Fields (activity_file);

			new_file->Write_Header ();
		}
	} else {
		new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);
	}
	if (detail_flag) {
		Header_Number (SHARE_DETAILS);
		New_Page ();
	}

	//---- process each trip ----

	if (activity_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", activity_file->File_Type (), activity_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", activity_file->File_Type ());
	}
	Set_Progress (10000);
	tour.hhold = tour.person = leg.hhold = leg.person = 0;
	prev_purp = -1;
	stat = true;

	while (stat) {

		//---- get the next activity record ----

		stat = activity_file->Read ();

		if (stat) {
			Show_Progress ();
			tot_act_in++;

			hhold = activity_file->Household ();
			if (hhold == 0) continue;

			if (hhlist_flag) {
				if (hhold_list.Get_Index (hhold) == 0) goto output;
			}
			person = activity_file->Person ();
		}

		//---- process the tour ----

		if (!stat || prev_purp == 0 || hhold != tour.hhold || person != tour.person) {
			if (method > 0 && tour.hhold > 0) {
				//Tour_Process (tour);

				if (new_act_flag) {
					new_file->Copy_Fields (activity_file);

					if (!new_file->Write ()) {
						Error ("Writing %s", new_file->File_Type ());
					}
				}
			}
			if (stat) break;

			//---- initialize the next tour ----

			if (hhold != tour.hhold) {
				if (!household_db.Get_Record (hhold)) {
					household_db.Reset_Record ();
				}
			}
			if (person_flag) {
				pop_rec.Household (hhold);
				pop_rec.Person (person);

				if (!person_db.Get_Record (pop_rec.ID ())) {
					person_db.Reset_Record ();
				}
			}
			tour.hhold = leg.hhold = hhold;
			tour.person = leg.person = person;
			prev_purp = -1;
		}

		//---- process the travel leg ----

		purpose = activity_file->Purpose ();
		destination = activity_file->Location ();

		if (prev_purp > 0) {
			leg.purpose = purpose;
			leg.trip = activity_file->Activity ();
			leg.mode = activity_file->Mode ();

			leg.destination = destination;
			time1 = time_step.Step (activity_file->Start_Min ());
			time2 = time_step.Step (activity_file->Start_Max ());

			if (time1 < 0 || time2 < 0) {
				Error ("Converting Start Time for Household %d", hhold);
			}
			leg.end = (time1 + time2 + 1) / 2;

			new_mode = Trip_Process (leg);
		} else {
			new_mode = 0;
		}

		//---- initialize the next travel leg ----

		leg.origin = destination;

		time1 = time_step.Step (activity_file->End_Min ());
		time2 = time_step.Step (activity_file->End_Max ());

		if (time1 < 0 || time2 < 0) {
			Error ("Converting End Time for Household %d", hhold);
		}
		leg.start = (time1 + time2 + 1) / 2;

		prev_purp = purpose;

		if (method > 0) continue;

		//---- update the mode data ----

		if (new_mode != 0 && new_mode != leg.mode) {
			leg.mode = new_mode;
			activity_file->Mode (new_mode);
			tot_act_mode++;
		}
		if (detail_flag) {
			Share_Details_Report (leg);
		}
		if (share_flag) {
			Write_Share_File (leg);
		}
output:
		if (new_act_flag) {
			new_file->Copy_Fields (activity_file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			tot_act_out++;
		}
	}
	End_Progress ();
	return (true);
}
