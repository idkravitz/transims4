//*********************************************************
//	Type_Data.cpp - survey type array class
//*********************************************************

#include "Type_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Type_Data constructor
//---------------------------------------------------------

Type_Data::Type_Data (int type) : Class_Index (type)
{
	Household (0);
	Num_Survey (0);
	Num_HHold (0);
	Cum_Weight (0.0);
}

//---------------------------------------------------------
//	Type_Array constructor
//---------------------------------------------------------

Type_Array::Type_Array (int max_records) : 
	Class_Array (sizeof (Type_Data), max_records)
{
}


