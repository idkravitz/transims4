//*********************************************************
//	TLen_Skim_Data.cpp - time length skim data classes
//*********************************************************

#include "TLen_Skim_Data.hpp"

//---------------------------------------------------------
//	Time_Len_Skim_Array constructor
//---------------------------------------------------------

Time_Len_Skim_Array::Time_Len_Skim_Array (int num_periods, int num_zones, int max_records) : 
	Index_Array (sizeof (Time_Len_Skim_Data), max_records, false), Matrix_Key (num_periods, num_zones)
{
}
