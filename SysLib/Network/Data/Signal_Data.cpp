//*********************************************************
//	Signal_Data.cpp - network signalized node data classes
//*********************************************************

#include "Signal_Data.hpp"

//---------------------------------------------------------
//	Signal_Data constructor
//---------------------------------------------------------

Signal_Data::Signal_Data (int node, int start) : 
	Class2_Index (node, start)
{
	Type (0);
	Rings (0);
	Groups (0);
	Timing (0);
	Offset (0);
	Coordinator (0);
}

//---------------------------------------------------------
//	Signal_Array constructor
//---------------------------------------------------------

Signal_Array::Signal_Array (int max_records) : 
	Class2_Array (sizeof (Signal_Data), max_records, false)
{
}
