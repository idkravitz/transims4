//*********************************************************
//	Household_Data.cpp - household data classes
//*********************************************************

#include "Household_Data.hpp"

//---------------------------------------------------------
//	Household_Data constructor
//---------------------------------------------------------

Household_Data::Household_Data (int id) : Class_Index (id)
{
	Location (0);
	Persons (0);
	Workers (0);
	Vehicles (0);
	Type (0);
}

//---------------------------------------------------------
//	Household_Array constructor
//---------------------------------------------------------

Household_Array::Household_Array (int max_records) : 
	Class_Array (sizeof (Household_Data), max_records, true)
{
}
