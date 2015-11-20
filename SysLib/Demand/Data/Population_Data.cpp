//*********************************************************
//	Population_Data.cpp - population data classes
//*********************************************************

#include "Population_Data.hpp"

//---------------------------------------------------------
//	Population_Data constructor
//---------------------------------------------------------

Population_Data::Population_Data (int hhold, int person) : 
	Class_Index (), Static_Scale ()
{
	Household (hhold);
	Person (person);
	Age (0);
	Gender (0);
	Work (0);
	Drive (0);
}

//---------------------------------------------------------
//	Population_Array constructor
//---------------------------------------------------------

Population_Array::Population_Array (int max_records) : 
	Class_Array (sizeof (Population_Data), max_records, true)
{
}
