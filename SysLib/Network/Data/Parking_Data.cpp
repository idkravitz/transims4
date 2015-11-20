//*********************************************************
//	Parking_Data.cpp - network parking data classes
//*********************************************************

#include "Parking_Data.hpp"

//---------------------------------------------------------
//	Parking_Data constructor
//---------------------------------------------------------

Parking_Data::Parking_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0);
	Type (0);
	Use (0);
	Space (0);
	Hourly (0);
	Daily (0);
	Start (0);
	End (0);
}

//---------------------------------------------------------
//	Parking_Array constructor
//---------------------------------------------------------

Parking_Array::Parking_Array (int max_records) : 
	Class_Array (sizeof (Parking_Data), max_records, true)
{
}

