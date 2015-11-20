//*********************************************************
//	HHold_Data.cpp - extended household data class
//*********************************************************

#include "HHold_Data.hpp"

//---------------------------------------------------------
//	HHold_Data constructor
//---------------------------------------------------------

HHold_Data::HHold_Data (void) : Household_Data (0)
{
	Vehicle_List (0);
	Problem_Flag (false);
}

//---------------------------------------------------------
//	HHold_Array constructor
//---------------------------------------------------------

HHold_Array::HHold_Array (int max_records) : 
	Household_Array (max_records)
{
	Data_Size (sizeof (HHold_Data));
}

