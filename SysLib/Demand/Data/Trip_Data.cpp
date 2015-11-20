//*********************************************************
//	Trip_Data.cpp - trip data classes
//*********************************************************

#include "Trip_Data.hpp"

//---------------------------------------------------------
//	Trip_Data constructor
//---------------------------------------------------------

Trip_Data::Trip_Data (int traveler, int trip) : 
	Class2_Index (traveler, trip), Static_Scale ()
{
	Purpose (0);
	Priority (0);
	Start_Time (0);
	End_Time (0);
	Mode (0);
	Vehicle (0);
	Origin (0);
	Destination (0);
	Constraint (0);
}

//---------------------------------------------------------
//	Trip_Array constructor
//---------------------------------------------------------

Trip_Array::Trip_Array (int max_records) : 
	Class2_Array (sizeof (Trip_Data), max_records, false)
{
}
