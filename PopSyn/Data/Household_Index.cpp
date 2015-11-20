//*********************************************************
//	Household_Index.cpp - household index by model cell
//*********************************************************

#include "Household_Index.hpp"

Household_Index Household_Index_Array::key;

//---------------------------------------------------------
//	Household_Index_Array constructor
//---------------------------------------------------------

Household_Index_Array::Household_Index_Array (int max_records) : 
	Complex_Array (sizeof (Household_Index), 2, false, max_records)
{
}

