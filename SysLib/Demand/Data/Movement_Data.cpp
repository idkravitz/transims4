//*********************************************************
//	Movement_Data.cpp - turn movement data class
//*********************************************************

#include "Movement_Data.hpp"

//---------------------------------------------------------
//	Movement_Array constructor
//---------------------------------------------------------

Movement_Array::Movement_Array (int max_records) :
	Complex_Array (sizeof (Movement_Data), 3, false, max_records, false)
{
}
