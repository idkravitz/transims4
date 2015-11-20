//*********************************************************
//	Zone_Location.cpp - zone location data class
//*********************************************************

#include "Zone_Location.hpp"

//---------------------------------------------------------
//	Zone_Location constructor
//---------------------------------------------------------

Zone_Location::Zone_Location (int zone) : Class_Index (zone)
{
	Location (0);
}

//---------------------------------------------------------
//	Zone_Location_Array constructor
//---------------------------------------------------------

Zone_Location_Array::Zone_Location_Array (int max_records) : 
	Class_Array (sizeof (Zone_Location), max_records)
{
}
