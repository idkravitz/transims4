//*********************************************************
//	Write_Riders.cpp - Write the New Ridership File
//*********************************************************

#include "RiderPrep.hpp"

//---------------------------------------------------------
//	Write_Riders
//---------------------------------------------------------

void RiderPrep::Write_Riders (void)
{
	int run, stop, time, schedule, load, board, alight, diff, last_diff;
	char mode [20];

	Ridership_File *file = (Ridership_File *) Demand_Db_Base (NEW_RIDERSHIP);

	Rider_Data *rider_ptr;

	//---- merge subarea ridership data ----

	Show_Message ("Writing %s -- Record", file->File_Type ());
	Set_Progress (10000);

	for (rider_ptr = rider_data.First (); rider_ptr; rider_ptr = rider_data.Next ()) {

		//---- convert the mode code to a string ----

		str_cpy (mode, sizeof (mode), Transit_Code ((Transit_Type) rider_ptr->Mode ()));

		//---- process each run stop ----

		for (run=1; run <= rider_ptr->Runs (); run++) {
			load = last_diff = 0;

			for (stop=1; stop <= rider_ptr->Stops (); stop++) {

				//---- check for riders ----
				
				board = rider_ptr->Board (run, stop);
				alight = rider_ptr->Alight (run, stop);

				load += board - alight;

				if (!stops_flag && load == 0 && board == 0 && alight == 0) continue;

				//---- calculate the travel time adjustment ----

				schedule = rider_ptr->Schedule (run, stop);
				diff = rider_ptr->Penalty (run, stop);

				if (diff != 0) {
					time = schedule + diff;
					last_diff = diff;
				} else if (last_diff != 0) {
					time = schedule + diff;
				} else {
					time = rider_ptr->Time (run, stop);
				}
				if (time == 0 && schedule != 0) {
					if (!stops_flag) continue;
					time = schedule;
				}

				//---- output the data record ----
				
				Show_Progress ();

				file->Mode (mode);
				file->Route (rider_ptr->Route ());
				file->Run (run);
				file->Stop (rider_ptr->Stop (stop));
				file->Schedule (trip_time.Format_Step (Resolve (schedule)));
				file->Time (trip_time.Format_Step (Resolve (time)));
				file->Board (board);
				file->Alight (alight);
				file->Load (load);
				file->Factor (0.0);

				if (!file->Write ()) {
					Error ("Writing Ridership Output File");
				}
			}
		}
	}
	End_Progress ();
} 
