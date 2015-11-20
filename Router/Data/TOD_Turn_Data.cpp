//*********************************************************
//	TOD_Turn_Data.cpp - turn restriction data classes
//*********************************************************

#include "TOD_Turn_Data.hpp"

//---------------------------------------------------------
//	TOD_Turn_Data constructor
//---------------------------------------------------------

TOD_Turn_Data::TOD_Turn_Data (void)
{
	Start (0);
	End (0);
	Use (0);
	Penalty (0);
	TOD_List (0);
}

//---------------------------------------------------------
//	TOD_Turn_Array constructor
//---------------------------------------------------------

TOD_Turn_Array::TOD_Turn_Array (int max_records) : 
	Data_Array (sizeof (TOD_Turn_Data), max_records)
{
}
