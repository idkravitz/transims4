//*********************************************************
//	Dir_Data.cpp - network link direction data classes
//*********************************************************

#include "Dir_Data.hpp"

//---------------------------------------------------------
//	Dir_Data constructor
//---------------------------------------------------------

Dir_Data::Dir_Data (int link_dir) : Class_Index (link_dir)
{
	Thru (0);
	Left (0);
	Right (0);
	Speed (0);
	Capacity (0);
	In_Bearing (0);
	Out_Bearing (0);
	Cost_List (0);
	TOD_List (0);
	Time0 (0);
}

//---------------------------------------------------------
//	Dir_Array constructor
//---------------------------------------------------------

Dir_Array::Dir_Array (int max_records) : 
	Class_Array (sizeof (Dir_Data), max_records, true)
{
}

