//*********************************************************
//	Signal_Offset.cpp - signal offset data class
//*********************************************************

#include "Signal_Offset.hpp"

//---------------------------------------------------------
//	Signal_Offset constructor
//---------------------------------------------------------

Signal_Offset::Signal_Offset (int signal) : Class_Index (signal)
{
	Period (0);
	Preset (0);
	Input (0);
	Offset (0);
	Cycle (0);
	group.Max_Records (8);
	phase.Max_Records (16);
}

//---------------------------------------------------------
//	Signal_Offset_Array constructor
//---------------------------------------------------------

Signal_Offset_Array::Signal_Offset_Array (int max_records) : 
	Class_Pointer (max_records, false)
{
}

void Signal_Offset_Array::Clear (void)
{
	for (Signal_Offset *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Class_Pointer::Clear ();
}

void Signal_Offset_Array::Reset (void)
{
	for (Signal_Offset *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Reset_Group ();
	}
}
