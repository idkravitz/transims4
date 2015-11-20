//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkData::Execute (void)
{
	//---- read the link node equivalence file ----

	Read_Equiv ();

	//---- read fthe directional data file ----

	Read_Data ();

	//---- write the link data file ----

	Write_Data ();

	Print (2, "Number of Link Node Equivalence Records = %d", nequiv);
	Print (1, "Number of From-To Link Combinations = %d", nab);
	Print (1, "Number of Directional Data Records = %d", ndir);
	Print (1, "Number of Link Data Records Written = %d", nlink);

	Exit_Stat (DONE);
}
