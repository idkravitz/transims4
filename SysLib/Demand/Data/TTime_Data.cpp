//*********************************************************
//	TTime_Data.cpp - network travel time class
//*********************************************************

#include "TTime_Data.hpp"

//---------------------------------------------------------
//	TTime_Data constructor
//---------------------------------------------------------

TTime_Data::TTime_Data (void) : Dir_Data (0), Lookup_Table (0, 0)
{
}

//---------------------------------------------------------
//	TTime_Array constructor
//---------------------------------------------------------

TTime_Array::TTime_Array (int max_records) : 
	Dir_Array (max_records), Lookup_Size (0, 2)
{
	Data_Size (sizeof (TTime_Data));
}

void TTime_Array::Clear (void)
{
	for (TTime_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}

