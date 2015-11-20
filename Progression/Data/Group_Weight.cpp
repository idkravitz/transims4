//*********************************************************
//	Group_Weight.cpp - group period weights
//*********************************************************

#include "Group_Weight.hpp"

//---------------------------------------------------------
//	Group_Weight constructor
//---------------------------------------------------------

Group_Weight::Group_Weight (int group) : Class_Index (group)
{
	weights.Initialize (sizeof (double), 10);
}

//---------------------------------------------------------
//	Group_Weight_Array constructor
//---------------------------------------------------------

Group_Weight_Array::Group_Weight_Array (int max_records) : 
	Class_Pointer (max_records, false)
{
}
void Group_Weight_Array::Clear ()
{
	for (Group_Weight *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Class_Pointer::Clear ();
}

