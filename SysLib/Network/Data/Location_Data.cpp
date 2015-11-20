//*********************************************************
//	Location_Data.cpp - network activity location classes
//*********************************************************

#include "Location_Data.hpp"

//---------------------------------------------------------
//	Location_Data constructor
//---------------------------------------------------------

Location_Data::Location_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0);
	X (0);
	Y (0);
	Zone (0);
}

//---------------------------------------------------------
//	Location_Array constructor
//---------------------------------------------------------

Location_Array::Location_Array (int max_records) : 
	Class_Array (sizeof (Location_Data), max_records, true)
{
}
