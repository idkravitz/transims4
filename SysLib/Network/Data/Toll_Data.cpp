//*********************************************************
//	Toll_Data.cpp - toll data classes
//*********************************************************

#include "Toll_Data.hpp"

//---------------------------------------------------------
//	Toll_Data constructor
//---------------------------------------------------------

Toll_Data::Toll_Data (int link_dir) : Class_Index (link_dir)
{
	Start (0);
	End (0);
	Use (0);
	Toll (0);
	TOD_List (0);
}

//---------------------------------------------------------
//	Toll_Array constructor
//---------------------------------------------------------

Toll_Array::Toll_Array (int max_records) : 
	Class_Array (sizeof (Toll_Data), max_records, false)
{
}
