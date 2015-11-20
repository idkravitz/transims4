//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcSnapshot.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcSnapshot::Execute (void)
{
	//---- read the network ----

	Demand_Service::Execute ();

	//---- set the subarea flag ----

	if (subarea_flag) {
		Set_Subarea ();
	}

	//---- process vehicle type data ----

	Set_Veh_Types ();

	//---- create the cell grid ----

	if (method == TOTAL || method == MAXIMUM) {
		Create_Cells ();

		Sum_Snapshot ();

	} else {

		//---- process the snapshot file ----

		Read_Snapshot ();
	}

	Exit_Stat (DONE);
}

