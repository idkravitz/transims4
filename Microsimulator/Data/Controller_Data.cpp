//*********************************************************
//	Controller_Data.cpp - traffic controller data classes
//*********************************************************

#include "Controller_Data.hpp"

//---------------------------------------------------------
//	Controller_Data constructor
//---------------------------------------------------------

Controller_Data::Controller_Data (int node) : Class_Index (node)
{
	End_Time (0);
	Type (0);
	Rings (0);
	Group (0);
	Groups (0);
	Movement (0);

	for (int i=1; i <= Max_Rings (); i++) {
		Phase (i, 0);
		Start (i, 0);
		Check (i, 0);
		Status (i, 0);
	}
}

//---------------------------------------------------------
//	Controller_Array constructor
//---------------------------------------------------------

Controller_Array::Controller_Array (int max_records) : 
	Class_Array (sizeof (Controller_Data), max_records)
{
}
