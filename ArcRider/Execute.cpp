//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcRider::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- write the line demands ----

	if (line_flag) {
		Write_Route ();
	}

	//---- write the line groups ----

	if (sum_flag) {
		Write_Sum ();
	}

	//---- allocate transit memory ----

	if (rider_flag || on_off_flag) {
		Setup_Riders ();
		Sum_Riders ();
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

	//---- write run capacity file ----

	if (cap_flag) {
		Write_Capacity ();
	}

	Write (1);
	if (line_flag) Write (1, "Number of ArcView Line Demand Shape Records = %d", num_line);
	if (sum_flag) Write (1, "Number of ArcView Line Group Shape Records = %d", num_sum);
	if (rider_flag) Write (1, "Number of ArcView Ridership Shape Records = %d", num_rider);
	if (demand_flag) Write (1, "Number of ArcView Stop Demand Shape Records = %d", num_stop);
	if (group_flag) Write (1, "Number of ArcView Stop Group Shape Records = %d", num_group);
	if (cap_flag) Write (1, "Number of ArcView Run Capacity Records = %d", num_cap);

	Exit_Stat (DONE);
}
