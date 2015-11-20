//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcPlan::Execute (void)
{
	int nfile;

	//---- read the network ----

	Demand_Service::Execute ();

	//---- allocate bandwidth memory ----

	if (width_flag) {
		int ndir = dir_data.Num_Records () + 1;

		width_data = new int [ndir];

		if (width_data == NULL) {
			Error ("Insufficient Memory for Bandwidth Data");
		}
		memset (width_data, '\0', ndir * sizeof (int));
	}

	//---- allocate contour memory ----

	if (time_flag || distance_flag) {
		int ndir = dir_data.Num_Records () + 1;

		link_flag = new bool [ndir];

		if (link_flag == NULL) {
			Error ("Insufficient Memory for Contour Data");
		}
		memset (link_flag, '\0', ndir * sizeof (bool));
	}

	//---- allocate transit memory ----

	if (rider_flag || on_off_flag) {
		Setup_Riders ();
	}

	//---- allocate parking memory ----

	if (parking_flag) {
		int nlots = parking_offset.Num_Records () + 1;

		parking_out = new int [nlots];
		parking_in = new int [nlots];

		if (parking_out == NULL || parking_in == NULL) {
			Error ("Insufficient Memory for Parking Demand Data");
		}
		memset (parking_out, '\0', nlots * sizeof (int));
		memset (parking_in, '\0', nlots * sizeof (int));
	}

	//---- read the household list ----

	if (hhlist_flag) {
		Read_HHList ();
	}

	//---- read the problem file ----

	if (problem_flag) {
		Read_Problem ();
	}

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		//---- process the plan files ----

		Read_Plan ();
	}

	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}
	if (plan_flag) {
		arcview_plan.Close ();
	}
	if (time_flag) {
		arcview_time.Close ();
	}
	if (distance_flag) {
		arcview_distance.Close ();
	}
	if (access_flag) {
		arcview_access.Close ();
	}

	//---- write bandwidth shapes ----

	if (width_flag) {
		Write_Bandwidth ();
	}

	//---- write ridership shapes ----

	if (rider_flag) {
		Write_Riders ();
	}

	//---- write transit stops file ----

	if (on_off_flag) {
		if (demand_flag) Write_Stops ();
		if (group_flag) Write_Group ();
	}

	//---- write parking lots file ----

	if (parking_flag) {
		Write_Parking ();
	}

	//---- write summary statistics ----

	Break_Check (9);
	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	Write (1);
	if (plan_flag) Write (1, "Number of ArcView Plan Shape Records = %d", num_out);
	if (problem_flag) Write (1, "Number of ArcView Problem Shape Records = %d", num_problem);
	if (width_flag) Write (1, "Number of ArcView Bandwidth Shape Records = %d", num_width);
	if (time_flag) Write (1, "Number of ArcView Time Contour Shape Records = %d", num_time);
	if (distance_flag) Write (1, "Number of ArcView Distance Contour Shape Records = %d", num_distance);
	if (access_flag) Write (1, "Number of ArcView Accessibility Shape Records = %d", num_access);
	if (rider_flag) Write (1, "Number of ArcView Ridership Shape Records = %d", num_rider);
	if (demand_flag) Write (1, "Number of ArcView Stop Demand Shape Records = %d", num_stop);
	if (group_flag) Write (1, "Number of ArcView Stop Group Shape Records = %d", num_group);
	if (parking_flag) Write (1, "Number of ArcView Parking Demand Shape Records = %d", num_parking);

	Exit_Stat (DONE);
}

