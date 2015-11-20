//*********************************************************
//	Subzone_Data.cpp - subzone data classes
//*********************************************************

#include "Subzone_Data.hpp"

//---------------------------------------------------------
//	Subzone_Data constructor
//---------------------------------------------------------

Subzone_Data::Subzone_Data (int zone, int id) : Class2_Index (zone, id)
{
	X (0);
	Y (0);
	Data (0);
}

//---------------------------------------------------------
//	Subzone_Array constructor
//---------------------------------------------------------

Subzone_Array::Subzone_Array (int max_records) : 
	Class2_Array (sizeof (Subzone_Data), max_records, true)
{
}
