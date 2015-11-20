//*********************************************************
//	Location_List.cpp - activity location data
//*********************************************************

#include "Location_List.hpp"

//---------------------------------------------------------
//	Location_List constructor
//---------------------------------------------------------

Location_List::Location_List (int id) : Class_Index (id)
{
	List (0);
	Parking (0);
	Num_Weight (0);
}

//---------------------------------------------------------
//	Location_List_Array constructor
//---------------------------------------------------------

Location_List_Array::Location_List_Array (int max_records) : 
	Class_Array (sizeof (Location_List), max_records)
{
	Num_Weight (0);
}

//---- num weight ----

void Location_List_Array::Num_Weight (int value)
{
	if (value > 0) {
		Data_Size (sizeof (Location_List) + (value - 1) * sizeof (double));
	}
	num_weight = value;
}

//---- new list ----

Location_List * Location_List_Array::New_List (int location)
{
	Location_List * list_ptr = New_Record (true);
	if (list_ptr == NULL) return (NULL);

	list_ptr->Location (location);
	list_ptr->Num_Weight (Num_Weight ());

	if (!Add ()) return (NULL);

	return (Record ());
}
