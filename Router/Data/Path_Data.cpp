//*********************************************************
//	Path_Data.cpp - path data classes
//*********************************************************

#include "Path_Data.hpp"

//---------------------------------------------------------
//	Path_Data constructor
//---------------------------------------------------------

Path_Data::Path_Data (void)
{
	Cum (0);
	TOD (0);
	Cost (0);
	Time (0);
	Prev (0);
	Next (0);
	Type (0);
	Xfer (0);
	Mode (0);
	Layer (0);
	Path (0);
}

//---------------------------------------------------------
//	Path_Array constructor
//---------------------------------------------------------

Path_Array::Path_Array (int max_records) : 
	Data_Array (sizeof (Path_Data), max_records)
{
}
