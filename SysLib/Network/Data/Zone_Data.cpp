//*********************************************************
//	Zone_Data.cpp - zone data classes
//*********************************************************

#include "Zone_Data.hpp"

//---------------------------------------------------------
//	Zone_Data constructor
//---------------------------------------------------------

Zone_Data::Zone_Data (int zone) : Class_Index (zone)
{
	X (0);
	Y (0);
	Area_Type (0);
}

//---------------------------------------------------------
//	Zone_Array constructor
//---------------------------------------------------------

Zone_Array::Zone_Array (int max_records) : 
	Class_Array (sizeof (Zone_Data), max_records, true)
{
}
