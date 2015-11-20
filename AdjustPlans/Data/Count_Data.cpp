//*********************************************************
//	Count_Data.cpp - traffic count data classes
//*********************************************************

#include "Count_Data.hpp"

//---------------------------------------------------------
//	Count_Data constructor
//---------------------------------------------------------

Count_Data::Count_Data (int dir, int time) : Class2_Index (dir, time)
{
	End_Time (0);
	Count (0);
	Volume (0);
}

//---------------------------------------------------------
//	Count_Array constructor
//---------------------------------------------------------

Count_Array::Count_Array (int max_records) : 
	Class2_Array (sizeof (Count_Data), max_records, false)
{
}
