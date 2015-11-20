//*********************************************************
//	Point_Time_Data.cpp - point data summary class
//*********************************************************

#include "Point_Time_Data.hpp"

//---------------------------------------------------------
//	Point_Time_Data constructor
//---------------------------------------------------------

Point_Time_Data::Point_Time_Data (void) : Class_Index (0), Lookup_Table (0, 0)
{
}

//---------------------------------------------------------
//	Point_Time_Array constructor
//---------------------------------------------------------

Point_Time_Array::Point_Time_Array (int max_records) : 
	Class_Array (max_records), Lookup_Size (0, 2)
{
	Data_Size (sizeof (Point_Time_Data));
}

void Point_Time_Array::Clear (void)
{
	for (Point_Time_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}
