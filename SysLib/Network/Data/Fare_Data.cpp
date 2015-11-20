//*********************************************************
//	Fare_Data.cpp - network transit fare data classes
//*********************************************************

#include "Fare_Data.hpp"

//---------------------------------------------------------
//	Fare_Array constructor
//---------------------------------------------------------

Fare_Array::Fare_Array (int max_records) : 
	Complex_Array (sizeof (Fare_Data), 2, false, max_records)
{
	time_period.Format ("24_HOUR_CLOCK");
}
