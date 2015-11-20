//*********************************************************
//	HHList_Data.cpp - household partition data
//*********************************************************

#include "HHList_Data.hpp"

//---------------------------------------------------------
//	HHList_Data constructor
//---------------------------------------------------------

HHList_Data::HHList_Data (int id) : Class_Index (id)
{
	File (0);
	Keep (false);
}

//---------------------------------------------------------
//	HHList_Array constructor
//---------------------------------------------------------

HHList_Array::HHList_Array (int max_records) : 
	Class_Array (sizeof (HHList_Data), max_records)
{
}
