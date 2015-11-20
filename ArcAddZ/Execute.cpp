//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcAddZ.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcAddZ::Execute (void)
{
	File_Group *group_ptr;

	//---- copy the shape files ----

	for (group_ptr = (File_Group *) file_group.First (); group_ptr; 
		group_ptr = (File_Group *) file_group.Next ()) {

		Copy_Shape (group_ptr);
	}
	Exit_Stat (DONE);
}

