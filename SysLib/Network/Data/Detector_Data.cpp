//*********************************************************
//	Detector_Data.cpp - network detector data classes
//*********************************************************

#include "Detector_Data.hpp"

//---------------------------------------------------------
//	Detector_Data constructor
//---------------------------------------------------------

Detector_Data::Detector_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0);
	Length (0);
	Low_Lane (0);
	High_Lane (0);
	Type (0);
	Coordinator (0);
}

//---------------------------------------------------------
//	Detector_Array constructor
//---------------------------------------------------------

Detector_Array::Detector_Array (int max_records) : 
	Class_Array (sizeof (Detector_Data), max_records, true)
{
}
