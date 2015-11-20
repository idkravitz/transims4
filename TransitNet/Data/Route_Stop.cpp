//*********************************************************
//	Route_Stop.cpp - route stop list
//*********************************************************

#include "Route_Stop.hpp"

//---------------------------------------------------------
//	Route_Stop constructor
//---------------------------------------------------------

Route_Stop::Route_Stop (void)
{
	Stop (0);
	Time (0);
	Zone (0);
}

//---------------------------------------------------------
//	Route_Stop_Array constructor
//---------------------------------------------------------

Route_Stop_Array::Route_Stop_Array (int max_records) : 
	Data_Array (sizeof (Route_Stop), max_records)
{
}
