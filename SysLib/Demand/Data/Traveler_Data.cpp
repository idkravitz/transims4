//*********************************************************
//	Traveler_Data.cpp - traveler data classes
//*********************************************************

#include "Traveler_Data.hpp"

//---------------------------------------------------------
//	Traveler_Data constructor
//---------------------------------------------------------

Traveler_Data::Traveler_Data (int traveler, int time) : 
	Class2_Index (traveler, time), Static_Scale ()
{
	Vehicle (0);
	Link_Dir (0);
	Offset (0);
	Lane (0);
	Distance (0);
	Speed (0);
}

//---------------------------------------------------------
//	Traveler_Array constructor
//---------------------------------------------------------

Traveler_Array::Traveler_Array (int max_records) : 
	Class2_Array (sizeof (Traveler_Data), max_records, false)
{
}
