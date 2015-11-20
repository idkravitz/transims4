//*********************************************************
//	Survey_HHold.cpp - extended household data class
//*********************************************************

#include "Survey_HHold.hpp"

//---------------------------------------------------------
//	Survey_HHold constructor
//---------------------------------------------------------

Survey_HHold::Survey_HHold (void) : Household_Data (0)
{
	Weight (0);
}

//---------------------------------------------------------
//	Survey_HHold_Array constructor
//---------------------------------------------------------

Survey_HHold_Array::Survey_HHold_Array (int max_records) : 
	Household_Array (max_records)
{
	Data_Size (sizeof (Survey_HHold));
}

