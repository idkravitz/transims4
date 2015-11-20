//*********************************************************
//	Activity_Data.cpp - activity data classes
//*********************************************************

#include "Activity_Data.hpp"

//---------------------------------------------------------
//	Activity_Data constructor
//---------------------------------------------------------

Activity_Data::Activity_Data (int traveler, int activity) : 
	Class2_Index (traveler, activity), Static_Scale ()
{
	Purpose (0);
	Priority (0);
	Start_Time (0);
	End_Time (0);
	Duration (0);
	Mode (0);
	Vehicle (0);
	Location (0);
	Passengers (0);
	Constraint (0);
}

//---------------------------------------------------------
//	Activity_Array constructor
//---------------------------------------------------------

Activity_Array::Activity_Array (int max_records) : 
	Class2_Array (sizeof (Activity_Data), max_records, false)
{
}
