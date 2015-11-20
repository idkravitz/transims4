//*********************************************************
//	Coordinator_Data.cpp - network signal coordinator data
//*********************************************************

#include "Coordinator_Data.hpp"

//---------------------------------------------------------
//	Coordinator_Data constructor
//---------------------------------------------------------

Coordinator_Data::Coordinator_Data (int id) : Class_Index (id)
{
}

//---------------------------------------------------------
//	Coordinator_Array constructor
//---------------------------------------------------------

Coordinator_Array::Coordinator_Array (int max_records) : 
	Class_Array (sizeof (Coordinator_Data), max_records, true)
{
}
