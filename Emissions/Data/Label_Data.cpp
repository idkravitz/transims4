//*********************************************************
//	Label_Data.cpp - category label data classes
//*********************************************************

#include "Label_Data.hpp"

//---------------------------------------------------------
//	Label_Data constructor
//---------------------------------------------------------

Label_Data::Label_Data (int value) : Class_Index (value)
{
	label [0] = '\0';
}

//---------------------------------------------------------
//	Label_Array constructor
//---------------------------------------------------------

Label_Array::Label_Array (int max_records) : 
	Class_Array (sizeof (Label_Data), max_records)
{
}
