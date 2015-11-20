//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SF3Prep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SF3Prep::Execute (void)
{
	SF3_GROUP *group;

	//---- copy the zone data ----

	if (input_flag) {
		Copy_Data ();
	}

	//---- read the SF3 groups ----

	for (group = (SF3_GROUP *) sf3_group.First (); group; group = (SF3_GROUP *) sf3_group.Next ()) {
		Read_SF3 (group);
	}

	//---- processing messages ----

	new_zone_file.Close ();

	Write (2, "Number of SF3 Data Records = %d", nrec);

	Write (1);
	if (input_flag) {
		Write (1, "Number of Input Zone Data Records = %d", zone_in);
	}
	Write (1, "Number of Output Zone_Data Records = %d", zone_out);
	if (input_flag) {
		Write (0, " (%d new)", zone_out - zone_in);
	}
	Exit_Stat (DONE);
}
