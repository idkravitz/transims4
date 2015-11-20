//*********************************************************
//	Event_Time.cpp - event time summary data
//*********************************************************

#include "Event_Time.hpp"

//---------------------------------------------------------
//	Event_Time constructor
//---------------------------------------------------------

Event_Time::Event_Time (void)
{
	Period (0);
	Trip_Start (0);
	Started (0);
	Trip_End (0);
	Ended (0);
	Mid_Trip (0);
	Sum_Trips (0);
	Travel_Time (0);
	Start_Diff (0);
	End_Diff (0);
	TTime_Diff (0);
	Start_Error (0);
	End_Error (0);
	TTime_Error (0);
	Sum_Error (0);
}

//---------------------------------------------------------
//	Event_Time_Array constructor
//---------------------------------------------------------

Event_Time_Array::Event_Time_Array (int max_records) : 
	Data_Array (sizeof (Event_Time), max_records)
{
}

