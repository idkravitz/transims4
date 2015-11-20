//*********************************************************
//	Boundary_Data.cpp - network travel time class
//*********************************************************

#include "Boundary_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Boundary_Data constructor
//---------------------------------------------------------

Boundary_Data::Boundary_Data (int lot) : Class_Index (lot), Lookup_Table (0, 0)
{
}

//---------------------------------------------------------
//	Boundary_Array constructor
//---------------------------------------------------------

Boundary_Array::Boundary_Array (int max_records) : 
	Class_Array (sizeof (Boundary_Data), max_records, true), Lookup_Size (0, 1)
{
}

void Boundary_Array::Clear (void)
{
	for (Boundary_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}
