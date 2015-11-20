//*********************************************************
//	Household_Type.cpp - household type array
//*********************************************************

#include "Household_Type.hpp"

//---------------------------------------------------------
//	Household_Type constructor
//---------------------------------------------------------

Household_Type::Household_Type (void) : HHRec_Index ()
{
	Type (0);
}

//---------------------------------------------------------
//	Household_Type_Array constructor
//---------------------------------------------------------

Household_Type_Array::Household_Type_Array (int max_records) : 
	HHRec_Array (sizeof (Household_Type), max_records)
{
}
