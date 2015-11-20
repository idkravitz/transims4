//*********************************************************
//	Time_Skim_Data.cpp - time skim data classes
//*********************************************************

#include "Time_Skim_Data.hpp"

//---------------------------------------------------------
//	Time_Skim_Array constructor
//---------------------------------------------------------

Time_Skim_Array::Time_Skim_Array (int num_periods, int num_zones, int max_records) : 
	Index_Array (sizeof (Time_Skim_Data), max_records, false), Matrix_Key (num_periods, num_zones)
{
}
