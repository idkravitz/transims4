//*********************************************************
//	Link_Use_Data.cpp - network link use data classes
//*********************************************************

#include "Link_Use_Data.hpp"

//---------------------------------------------------------
//	Link_Use_Data constructor
//---------------------------------------------------------

Link_Use_Data::Link_Use_Data (void)
{
	Use (0);
	Lanes (0);
	HOV_Lanes (0);
	Start (0);
	End (0);
	TOD_List (0);
	TOD_Use (0);
}

//---------------------------------------------------------
//	Link_Use_Array constructor
//---------------------------------------------------------

Link_Use_Array::Link_Use_Array (int max_records) : 
	Data_Array (sizeof (Link_Use_Data), max_records)
{
}
