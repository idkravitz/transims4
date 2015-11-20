//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void NewFormat::Execute (void)
{

	//---- read the network ----

	Demand_Service::Execute ();

	//---- convert the driver plan file ----

	if (driver_flag) {
		Driver_Plan ();
	}

	//---- convert the population file ----

	if (population_flag) {
		Population ();
	}

	//---- convert the activity file ----

	if (activity_flag) {
		Activity ();
	}

	//---- convert the survey activity ----

	if (survey_flag) {
		Survey_Activity ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}
