//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "ModeChoice.hpp"

#include "Trip_File.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

bool ModeChoice::Read_Trip (int file_num)
{
	int hhold, person, new_mode;

	Trip_File *trip_file, *new_file;
	Population_Data pop_rec;
	Travel_Leg leg;
	
	trip_file = (Trip_File *) Demand_Db_Base (TRIP);
		
	if (file_num > 0) {
		if (!trip_file->Open (file_num)) return (false);

		if (new_trip_flag) {
			new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

			if (!new_file->Open (file_num)) {
				Error ("Creating %s", new_file->File_Type ());
			}
			new_file->Clear_Fields ();
			new_file->Replicate_Fields (trip_file);

			new_file->Write_Header ();
		}
	} else if (new_trip_flag) {
		new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);
	}

	if (detail_flag) {
		Header_Number (SHARE_DETAILS);
		New_Page ();
	}

	//---- process each trip ----

	if (trip_file->Extend ()) {
		Show_Message ("Reading %s %s -- Record", trip_file->File_Type (), trip_file->Extension ());
	} else {
		Show_Message ("Reading %s -- Record", trip_file->File_Type ());
	}
	Set_Progress (10000);
	leg.hhold = leg.person = 0;

	while (trip_file->Read ()) {
		Show_Progress ();
		tot_trip_in++;

		hhold = trip_file->Household ();
		if (hhold == 0) continue;

		if (hhlist_flag) {
			if (hhold_list.Get_Index (hhold) == 0) goto output;
		}
		person = trip_file->Person ();

		if (hhold != leg.hhold || person != leg.person) {
			if (hhold != leg.hhold) {
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
			leg.hhold = hhold;
			leg.person = person;
		}
		leg.trip = trip_file->Trip ();
		leg.purpose = trip_file->Purpose ();
		leg.mode = trip_file->Mode ();
		leg.origin = trip_file->Origin ();
		leg.destination = trip_file->Destination ();

		leg.start = time_step.Step (trip_file->Start ());
		if (leg.start < 0) {
			Error ("Converting Start Time %s for Household %d", trip_file->Start (), hhold);
		}
		leg.end = time_step.Step (trip_file->Arrive ());
		if (leg.end < 0) {
			Error ("Converting Arrival Time %s for Household %d", trip_file->Arrive (), hhold);
		}
		
		new_mode = Trip_Process (leg);

		if (new_mode != 0 && new_mode != leg.mode) {
			leg.mode = new_mode;
			trip_file->Mode (new_mode);
			tot_trip_mode++;
		}
		if (leg.org_zone == leg.des_zone) {
			tot_intra++;
		}
		if (detail_flag) {
			Share_Details_Report (leg);
		}
		if (share_flag) {
			Write_Share_File (leg);
		}

output:
		if (new_trip_flag) {
			new_file->Copy_Fields (trip_file);

			if (!new_file->Write ()) {
				Error ("Writing %s", new_file->File_Type ());
			}
			tot_trip_out++;
		}
	}
	End_Progress ();
	Header_Number (0);
	return (true);
}
