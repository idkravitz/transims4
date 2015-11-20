//*********************************************************
//	Access_Data.cpp - network process link data classes
//*********************************************************

#include "Access_Data.hpp"

//---------------------------------------------------------
//	Access_Data constructor
//---------------------------------------------------------

Access_Data::Access_Data (int id) : Class_Index (id)
{
	From_ID (0);
	To_ID (0);
	From_Type (0);
	To_Type (0);
	Time (0);
	Cost (0);
}

//---------------------------------------------------------
//	Access_Array constructor
//---------------------------------------------------------

Access_Array::Access_Array (int max_records) : 
	Class_Array (sizeof (Access_Data), max_records, true)
{
}

