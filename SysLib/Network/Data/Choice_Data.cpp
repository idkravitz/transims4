//*********************************************************
//	Choice_Data.cpp - extended location data class
//*********************************************************

#include "Choice_Data.hpp"

//---------------------------------------------------------
//	Choice_Data constructor
//---------------------------------------------------------

Choice_Data::Choice_Data (void) : Location_Data (0)
{
	Mode (0);
	List (0);
	Utility (0.0);
}

//---------------------------------------------------------
//	Choice_Array constructor
//---------------------------------------------------------

Choice_Array::Choice_Array (int max_records) : 
	Location_Array (max_records)
{
	Data_Size (sizeof (Choice_Data));
}

