//*********************************************************
//	Zone_List.cpp - extended zone data class
//*********************************************************

#include "Zone_List.hpp"

//---------------------------------------------------------
//	Zone_List constructor
//---------------------------------------------------------

Zone_List::Zone_List (void) : Zone_Data (0)
{
	Location (0);
}

//---------------------------------------------------------
//	Zone_List_Array constructor
//---------------------------------------------------------

Zone_List_Array::Zone_List_Array (int max_records) : 
	Zone_Array (max_records)
{
	Data_Size (sizeof (Zone_List));
}

