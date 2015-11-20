//*********************************************************
//	Offset_Data.cpp - link offset data
//*********************************************************

#include "Offset_Data.hpp"

//---------------------------------------------------------
//	Offset_Data constructor
//---------------------------------------------------------

Offset_Data::Offset_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0.0);
	X (0.0);
	Y (0.0);
	Z (0.0);
}

//---------------------------------------------------------
//	Offset_Array constructor
//---------------------------------------------------------

Offset_Array::Offset_Array (int max_records) :
	Class_Array (sizeof (Offset_Data), max_records, true)
{
}
