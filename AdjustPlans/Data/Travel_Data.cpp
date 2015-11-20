//*********************************************************
//	Travel_Data.cpp - plan travel data classes
//*********************************************************

#include "Travel_Data.hpp"

//---------------------------------------------------------
//	Travel_Data constructor
//---------------------------------------------------------

Travel_Data::Travel_Data (int traveler) : Class_Index (traveler), Static_Scale ()
{
	Weight (0);
	count.Clear ();
}

//---------------------------------------------------------
//	Travel_Array constructor
//---------------------------------------------------------

Travel_Array::Travel_Array (int max_records) : 
	Class_Array (sizeof (Travel_Data), max_records, false)
{
}
