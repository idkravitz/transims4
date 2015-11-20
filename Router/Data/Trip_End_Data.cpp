//*********************************************************
//	Trip_End_Data.cpp - trip end data classes
//*********************************************************

#include "Trip_End_Data.hpp"

//---------------------------------------------------------
//	Trip_End_Data constructor
//---------------------------------------------------------

Trip_End_Data::Trip_End_Data (void)
{
	Rec (0);
	ID (0);
	Type (0);
	Link (0);
	Cum (0);
	TOD (0);
	Cost (0);
	Time (0);
	Prev (0);
	Layer (0);
	Path (0);
}

//---------------------------------------------------------
//	Trip_End_Array constructor
//---------------------------------------------------------

Trip_End_Array::Trip_End_Array (int max_records) : 
	Data_Array (sizeof (Trip_End_Data), max_records)
{
}
