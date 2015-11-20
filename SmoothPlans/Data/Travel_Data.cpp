//*********************************************************
//	Travel_Data.cpp - plan travel data classes
//*********************************************************

#include "Travel_Data.hpp"

//---------------------------------------------------------
//	Travel_Data constructor
//---------------------------------------------------------

Travel_Data::Travel_Data (int traveler) : Class_Index (traveler)
{
	Change (0);
	Min_Change (0);
	Max_Change (0);
	Next_Index (0);
	Flag (false);
	dir.Clear ();
}

//---------------------------------------------------------
//	Travel_Array constructor
//---------------------------------------------------------

Travel_Array::Travel_Array (int max_records) : 
	Class_Array (sizeof (Travel_Data), max_records, false)
{
}
