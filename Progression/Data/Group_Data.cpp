//*********************************************************
//	Group_Data.cpp - network grid data class
//*********************************************************

#include "Group_Data.hpp"

//---------------------------------------------------------
//	Group_Data constructor
//---------------------------------------------------------

Group_Data::Group_Data (int group) : Class_Index (group)
{
	Next_Group (0);
	link.Max_Records (20);
}

//---------------------------------------------------------
//	Group_Array constructor
//---------------------------------------------------------

Group_Array::Group_Array (int max_records) : 
	Class_Pointer (max_records, false)
{
}

void Group_Array::Clear (void)
{
	for (Group_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Class_Pointer::Clear ();
}

void Group_Array::Reset (void)
{
	for (Group_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Reset ();
	}
	Class_Pointer::Reset ();
}
