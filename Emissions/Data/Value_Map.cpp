//*********************************************************
//	Value_Map.cpp - summary value map classes
//*********************************************************

#include "Value_Map.hpp"

//---------------------------------------------------------
//	Value_Map constructor
//---------------------------------------------------------

Value_Map::Value_Map (int value, int member) : Class2_Index (value, member)
{
	Group (0);
	Weight (0.0);
}

//---------------------------------------------------------
//	Value_Map_Array constructor
//---------------------------------------------------------

Value_Map_Array::Value_Map_Array (int max_records) : 
	Class2_Array (sizeof (Value_Map), max_records)
{
}
