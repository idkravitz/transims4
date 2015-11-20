//*********************************************************
//	Link_Time.cpp - link and time period data classes
//*********************************************************

#include "Link_Time.hpp"

//---------------------------------------------------------
//	Link_Time constructor
//---------------------------------------------------------

Link_Time::Link_Time (void)
{
	Dir_Index (0);
	Time_Period (0);
}

//---------------------------------------------------------
//	Link_Time_Array constructor
//---------------------------------------------------------

Link_Time_Array::Link_Time_Array (void) : 
	Data_Array (sizeof (Link_Time), 15)
{
}
