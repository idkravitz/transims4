//*********************************************************
//	Traveler_Data.cpp - traveler data classes
//*********************************************************

#include "Traveler_Data.hpp"

//---------------------------------------------------------
//	Traveler_Data constructor
//---------------------------------------------------------

Traveler_Data::Traveler_Data (int traveler) : Class_Index (traveler)
{
	Active (false);
	Problem (false);
	End_Time (0);
	Next_Rider (0);
	Travel_Plan (NULL);
}

//---------------------------------------------------------
//	Traveler_Array constructor
//---------------------------------------------------------

Traveler_Array::Traveler_Array (int max_records) : 
	Class_Array (sizeof (Traveler_Data), max_records)
{
}
