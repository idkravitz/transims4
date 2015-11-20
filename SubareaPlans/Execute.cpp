//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SubareaPlans::Execute (void)
{
	int nfile;
	double total;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- set the boundary flags ----

	Set_Boundary ();

	//---- write the boundary records ----

	Write_Boundary ();

	//---- create subarea transit data ----

	if (transit_flag) {
		Create_Route ();
	}
	if (output_flag) {
		Break_Check (5);
		Write (2, "Number of Subarea Transit Stop Records = %d", nstop);
		Write (1, "Number of Subarea Transit Route Records = %d", nroute);
		Write (1, "Number of Subarea Transit Schedule Records = %d", nschedule);
		Write (1, "Number of Subarea Transit Driver Records = %d", ndriver);
	}

	Break_Check (6);
	Print (1);
	Write (1, "Number of Boundary Links = %d", nlink);
	Write (1, "Number of Short Links Skipped = %d", nshort);
	Write (1, "Number of New Activity Locations = %d", nlocation);
	Write (1, "Number of New Parking Lots = %d", nparking);
	Write (1, "Number of New Process Links = %d", naccess);
	if (transit_flag) {
		Write (1, "Number of New Transit Stops = %d", new_stop);
		Write (1, "Number of New Transit Routes = %d", new_route);
	}
	Show_Message (1);

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!input_plans.Open (nfile)) break;

		if (extend_flag && nfile > 0) {
			if (!subarea_plans.Open (nfile)) {
				Error ("Opening Subarea Plan File %s", subarea_plans.Extension ());
			}
		}
		if (hhlist_flag && hhlist_file.Extend () > 0 && nfile > 0) {
			if (!hhlist_file.Open (nfile)) {
				Error ("Opening Subarea Household List %s", hhlist_file.Extension ());
			}
		}

		//---- process the plan files ----

		Read_Plans ();
	}

	//---- close the files ----

	subarea_plans.Close ();

	if (hhlist_flag) {
		hhlist_file.Close ();
	}
	if (nfile == 0) {
		File_Error ("No Plan Files were Found", input_plans.Filename ());
	}

	//---- write the subarea vehicle file ----

	Write_Vehicle ();

	//---- write the summary report ----

	if (Report_Flag (TRIP_REPORT)) {
		Trip_Report ();
	}

	//---- write summary statistics ----

	Break_Check (6);
	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", input_plans.Num_Plans ());
	Write (1, "Number of Input Records = %d", input_plans.Num_Records ());
	Write (1, "Number of Input Travelers = %d", input_plans.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", input_plans.Num_Trips ());

	Break_Check (6);
	Write (2, "Number of Subarea Files = %d", ((extend_flag) ? nfile : 1));
	Write (1, "Number of Subarea Plans = %d", subarea_plans.Num_Plans ());
	Write (1, "Number of Subarea Records = %d", subarea_plans.Num_Records ());
	Write (1, "Number of Subarea Travelers = %d", subarea_plans.Num_Travelers ());
	Write (1, "Number of Subarea Trips = %d", subarea_plans.Num_Trips ());

	if (num_adjust > 0) {
		total_adjust = (total_adjust + num_adjust / 2) / num_adjust;

		Break_Check (3);
		Write (2, "Number of Plans with Adjusted Times = %d", num_adjust);
		Write (1, "Average Travel Time Adjustment = %d seconds (%.1lf minutes)", 
			total_adjust, (total_adjust / 60.0));
	}

	//---- summarize transit legs ----

	total = leg_in_in + leg_in_out + leg_out_in + leg_out_out;

	if (total > 0) {
		total = 100.0 / total;

		Break_Check (5);
		Write (2, "Number of Internal-Internal Transit Legs = %d (%.1lf%%)", leg_in_in, (leg_in_in * total));
		Write (1, "Number of Internal-External Transit Legs = %d (%.1lf%%)", leg_in_out, (leg_in_out * total));
		Write (1, "Number of External-Internal Transit Legs = %d (%.1lf%%)", leg_out_in, (leg_out_in * total));
		Write (1, "Number of External-External Transit Legs = %d (%.1lf%%)", leg_out_out, (leg_out_out * total));
	}

	//---- summarize drive legs ----

	total = num_in_in + num_in_out + num_out_in + num_out_out;

	if (total > 0) {
		total = 100.0 / total;

		Break_Check (5);
		Write (2, "Number of Internal-Internal Vehicle Trips = %d (%.1lf%%)", num_in_in, (num_in_in * total));
		Write (1, "Number of Internal-External Vehicle Trips = %d (%.1lf%%)", num_in_out, (num_in_out * total));
		Write (1, "Number of External-Internal Vehicle Trips = %d (%.1lf%%)", num_out_in, (num_out_in * total));
		Write (1, "Number of External-External Vehicle Trips = %d (%.1lf%%)", num_out_out, (num_out_out * total));
	}
	Exit_Stat (DONE);
}
