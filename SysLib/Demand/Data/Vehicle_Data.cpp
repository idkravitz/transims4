//*********************************************************
//	Vehicle_Data.cpp - vehicle data classes
//*********************************************************

#include "Vehicle_Data.hpp"

//---------------------------------------------------------
//	Vehicle_Data constructor
//---------------------------------------------------------

Vehicle_Data::Vehicle_Data (int vehicle) : Class_Index (vehicle)
{
	Household (0);
	Location (0);
	Type (0);
	Sub_Type (0);
	Passengers (0);
}

//---------------------------------------------------------
//	Vehicle_Array constructor
//---------------------------------------------------------

Vehicle_Array::Vehicle_Array (int max_records) : 
	Class_Array (sizeof (Vehicle_Data), max_records)
{
}
