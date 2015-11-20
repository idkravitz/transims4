//*********************************************************
//	Integer_Array.cpp - dynamically allocated array
//*********************************************************

#include "Integer_Array.hpp"

//---------------------------------------------------------
//	Integer_List constructor
//---------------------------------------------------------

Integer_List::Integer_List (int max_records) : 
	Data_Array (sizeof (int), max_records)
{
}

//---------------------------------------------------------
//	Integer_Sort constructor
//---------------------------------------------------------

Integer_Sort::Integer_Sort (int max_records, bool sorted) : 
	Index_Array (sizeof (int), max_records, sorted)
{
}
