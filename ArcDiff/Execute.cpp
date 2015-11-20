//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcDiff.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcDiff::Execute (void)
{
	Delay_Group *group_ptr;

	//---- merge link delay files ----

	for (group_ptr = (Delay_Group *) delay_group.First (); group_ptr; 
		group_ptr = (Delay_Group *) delay_group.Next ()) {

		Merge_Delay (group_ptr);
	}
	Exit_Stat (DONE);
}

