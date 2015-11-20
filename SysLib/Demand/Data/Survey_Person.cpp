//*********************************************************
//	Survey_Person.cpp - extended population data class
//*********************************************************

#include "Survey_Person.hpp"

//---------------------------------------------------------
//	Survey_Person constructor
//---------------------------------------------------------

Survey_Person::Survey_Person (void) : Population_Data ()
{
	Count (0);
}

//---------------------------------------------------------
//	Survey_Person_Array constructor
//---------------------------------------------------------

Survey_Person_Array::Survey_Person_Array (int max_records) : 
	Population_Array (max_records)
{
	Data_Size (sizeof (Survey_Person));
}

