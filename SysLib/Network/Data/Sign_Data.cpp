//*********************************************************
//	Sign_Data.cpp - network unsignalized node data classes
//*********************************************************

#include "Sign_Data.hpp"

//---------------------------------------------------------
//	Sign_Data constructor
//---------------------------------------------------------

Sign_Data::Sign_Data (int link_dir) : Class_Index (link_dir)
{
	Sign (0);
}

//---------------------------------------------------------
//	Sign_Array constructor
//---------------------------------------------------------

Sign_Array::Sign_Array (int max_records) : 
	Class_Array (sizeof (Sign_Data), max_records, false)
{
}
