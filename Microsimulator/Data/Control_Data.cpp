//*********************************************************
//	Control_Data.cpp - traffic control data classes
//*********************************************************

#include "Control_Data.hpp"

//---------------------------------------------------------
//	Control_Data constructor
//---------------------------------------------------------

Control_Data::Control_Data (int in_link_dir, int out_link_dir) : Class2_Index (in_link_dir, out_link_dir)
{
	Control_Status (0);
	Approach_Dir (0);

	for (int i=1; i <= Max_Conflicts (); i++) {
		Conflict_Dir (i, 0);
		Low_Lane (i, 0);
		High_Lane (i, 0);
	}
}

//---------------------------------------------------------
//	Control_Array constructor
//---------------------------------------------------------

Control_Array::Control_Array (int max_records) : 
	Class2_Array (sizeof (Control_Data), max_records, false)
{
}
