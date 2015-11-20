//*********************************************************
//	Factor_Data.cpp - factor data classes
//*********************************************************

#include "Factor_Data.hpp"

//---------------------------------------------------------
//	Factor_Data constructor
//---------------------------------------------------------

Factor_Data::Factor_Data (int key) : Class_Index (key)
{
	Factor (0.0);
	Bucket (0.45);
}

int Factor_Data::Bucket_Factor (double value)
{
	int lvalue;

	value = value * factor + bucket;
	lvalue = (int) value;
	bucket = (float) (value - lvalue);

	return (lvalue);
}

//---------------------------------------------------------
//	Factor_Array constructor
//---------------------------------------------------------

Factor_Array::Factor_Array (int num_periods, int num_zones, int max_records) : 
	Class_Array (sizeof (Factor_Data), max_records, false), Matrix_Key (num_periods, num_zones)
{
}
