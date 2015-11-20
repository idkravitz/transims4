//*********************************************************
//	Weight_Data.cpp - survey household weights class
//*********************************************************

#include "Weight_Data.hpp"

//---------------------------------------------------------
//	Weight_Data constructor
//---------------------------------------------------------

Weight_Data::Weight_Data (int id) : Class_Index (id)
{
	Weight (0.0);
}

//---------------------------------------------------------
//	Weight_Array constructor
//---------------------------------------------------------

Weight_Array::Weight_Array (int max_records) : 
	Class_Array (sizeof (Weight_Data), max_records)
{
}

