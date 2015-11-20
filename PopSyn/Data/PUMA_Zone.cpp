//*********************************************************
//	PUMA_Zone.cpp - PUMA zone data class
//*********************************************************

#include "PUMA_Zone.hpp"

//---------------------------------------------------------
//	PUMA_Zone constructor
//---------------------------------------------------------

PUMA_Zone::PUMA_Zone (int zone) : Class_Index (zone)
{
	data [0] = 0.0;
}

//---------------------------------------------------------
//	PUMA_Zone_Array constructor
//---------------------------------------------------------

PUMA_Zone_Array::PUMA_Zone_Array (int max_records) : 
	Class_Array (sizeof (PUMA_Zone), max_records)
{
}
