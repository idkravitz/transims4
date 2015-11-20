//*********************************************************
//	Lane_Use_Data.cpp - network lane use data classes
//*********************************************************

#include "Lane_Use_Data.hpp"

//---------------------------------------------------------
//	LaneUse_Data constructor
//---------------------------------------------------------

Lane_Use_Data::Lane_Use_Data (int link_dir) : Class_Index (link_dir)
{
	Lane (0);
	Type (0);
	Use (0);
	Start (0);
	End (0);
	Offset (0);
	Length (0);
}

//---------------------------------------------------------
//	Lane_Use_Array constructor
//---------------------------------------------------------

Lane_Use_Array::Lane_Use_Array (int max_records) : 
	Class_Array (sizeof (Lane_Use_Data), max_records, false)
{
}
