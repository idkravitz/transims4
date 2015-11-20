//*********************************************************
//	Route_Stop_Data.cpp - transit route stop data classes
//*********************************************************

#include "Route_Stop_Data.hpp"

//---------------------------------------------------------
//	Route_Stop_Data constructor
//---------------------------------------------------------

Route_Stop_Data::Route_Stop_Data (void)
{
	Route (0);
	Stop (0);
	List (0);
}

//---------------------------------------------------------
//	Route_Stop_Array constructor
//---------------------------------------------------------

Route_Stop_Array::Route_Stop_Array (int max_records) : 
	Data_Array (sizeof (Route_Stop_Data), max_records)
{
}
