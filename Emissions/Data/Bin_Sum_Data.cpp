//*********************************************************
//	Bin_Sum_Data.cpp - emissions summary data classes
//*********************************************************

#include "Bin_Sum_Data.hpp"

//---------------------------------------------------------
//	Bin_Sum_Data constructor
//---------------------------------------------------------

Bin_Sum_Data::Bin_Sum_Data () : Class_Index (0)
{
	meters = 0.0;
	seconds = 0.0;
}

//---- Sum ----

void Bin_Sum_Data::Sum (Bin_Sum_Data *ptr) 
{
	meters += ptr->Meters ();
	seconds += ptr->Seconds ();
}

//---- Zero ----

void Bin_Sum_Data::Zero (Bin_Sum_Data *ptr) 
{
	if (ptr != NULL) {
		ID (ptr->ID ());
	} else {
		ID (0);
	}
	meters = seconds = 0.0;
}

//---------------------------------------------------------
//	Bin_Sum_Array constructor
//---------------------------------------------------------

Bin_Sum_Array::Bin_Sum_Array (int max_records) : 
	Class_Array (sizeof (Bin_Sum_Data), max_records)
{
}

