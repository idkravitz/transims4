//*********************************************************
//	Pocket_Data.cpp - network pocket lane data classes
//*********************************************************

#include "Pocket_Data.hpp"

//---------------------------------------------------------
//	Pocket_Data constructor
//---------------------------------------------------------

Pocket_Data::Pocket_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0);
	Lane (0);
	Type (0);
	Length (0);
}

//---------------------------------------------------------
//	Pocket_Array constructor
//---------------------------------------------------------

Pocket_Array::Pocket_Array (int max_records) : 
	Class_Array (sizeof (Pocket_Data), max_records, true)
{
}
