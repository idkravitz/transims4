//*********************************************************
//	List_Data.cpp - list data classes
//*********************************************************

#include "List_Data.hpp"

//---------------------------------------------------------
//	List_Data constructor
//---------------------------------------------------------

List_Data::List_Data (void)
{
	From_List (0);
	To_List (0);
}

//---------------------------------------------------------
//	List_Array constructor
//---------------------------------------------------------

List_Array::List_Array (int max_records) : 
	Data_Array (sizeof (List_Data), max_records)
{
}
