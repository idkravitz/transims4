//*********************************************************
//	Timing_Data.cpp - network timing plan data classes
//*********************************************************

#include "Timing_Data.hpp"

//---------------------------------------------------------
//	Timing_Data constructor
//---------------------------------------------------------

Timing_Data::Timing_Data (void) : Class_Index (0)
{
	Timing (0);
	Phase (0);
	Next_Phase (0);
	Min_Green (0);
	Max_Green (0);
	Ext_Green (0);
	Yellow (0);
	Red_Clear (0);
	Ring (0);
	Group (0);
}

//---------------------------------------------------------
//	Timing_Array constructor
//---------------------------------------------------------

Timing_Array::Timing_Array (int max_records) : 
	Class_Array (sizeof (Timing_Data), max_records, false)
{
}
