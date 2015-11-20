//*********************************************************
//	Type_Map.cpp - map for types and subtypes
//*********************************************************

#include "Type_Map.hpp"

//---------------------------------------------------------
//	Type_Map constructor
//---------------------------------------------------------

Type_Map::Type_Map (int type, int subtype) : Class_Index (0)
{
	Type (type);
	SubType (subtype);
	Map (0);
}

//---------------------------------------------------------
//	Type_Map_Array constructor
//---------------------------------------------------------

Type_Map_Array::Type_Map_Array (int max_records) : 
	Class_Array (sizeof (Type_Map), max_records)
{
}
