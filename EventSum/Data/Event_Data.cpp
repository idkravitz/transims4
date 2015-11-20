//*********************************************************
//	Event_Data.cpp - trip event data
//*********************************************************

#include "Event_Data.hpp"

//---------------------------------------------------------
//	Event_Data constructor
//---------------------------------------------------------

Event_Data::Event_Data (void) : Class2_Index (), Static_Scale ()
{
	Household (0);
	Person (0);
	Trip (0);
	Leg (0);
	Mode (0);
	Purpose (0);
	Constraint (0);
	Start_Link (0);
	End_Link (0);
	Trip_Start (0);
	Trip_End (0);
	Base_Start (0);
	Base_End (0);
	Started (0);
	Ended (0);
}

//---------------------------------------------------------
//	Event_Array constructor
//---------------------------------------------------------

Event_Array::Event_Array (int max_records) : 
	Class2_Array (sizeof (Event_Data), max_records)
{
}
