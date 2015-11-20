//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Relocate::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the zone list ----

	if (zone_flag) {
		Read_Zone_List ();
	}

	//---- map the network data ----

	if (network_flag) {
		Map_Network ();
	}

	//---- match activity locations ----

	Match_Locations ();

	//---- check for special partition processing ----

	if (loop_flag) {

		//---- process trip and activity files by partition ----

		for (nfile=0; ; nfile++) {
			bool trip_stat, act_stat;
			
			if (nfile > 0) {
				trip_stat = trip_file->Open (nfile);
				act_stat = activity_file->Open (nfile);

				if (!trip_stat && !act_stat) break;

				if (!hhlist_file.Open (nfile)) goto hhlist_error;
			} else {
				trip_stat = act_stat = true;
			}
			if (trip_stat) Read_Trip (nfile);
			if (act_stat) Read_Activity (nfile);
		}

	} else {

		//---- process trip files ----

		if (trip_flag) {
			for (nfile=0; ; nfile++) {
				if (nfile > 0) {
					if (!trip_file->Open (nfile)) break;
					if (hhlist_flag && !hhlist_file.Open (nfile)) goto hhlist_error;
				}
				Read_Trip (nfile);
			}
		}

		//---- process activity files ----

		if (activity_flag) {
			for (nfile=0; ; nfile++) {
				if (nfile > 0) {
					if (!activity_file->Open (nfile)) break;
					if (hhlist_flag && !hhlist_file.Open (nfile)) goto hhlist_error;
				}
				Read_Activity (nfile);
			}
		}
	}

	//---- process plan files ----

	if (plan_flag) {
		total_hhold = 0;

		for (nfile=0; ; nfile++) {
			if (!Read_Plan (nfile)) break;
		}
	}

	//---- process the vehicle file ----

	if (vehicle_flag) {
		Read_Vehicle ();
	}

	//---- summarize the results ----

	Write (2, "Total Number of Records Read = %d", total_in);
	Write (1, "Total Number of Records Written = %d", total_out);
	Write (1, "Total Number of Records Updated = %d", total_update);

	if (total_hhold > 0) {
		Write (1, "Number of Household List Records = %d", total_hhold);
	}
	Exit_Stat (DONE);

hhlist_error:
	Error ("Creating %s", hhlist_file.File_Type ());
}
