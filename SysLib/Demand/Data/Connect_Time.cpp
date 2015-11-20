//*********************************************************
//	Connect_Time.cpp - connection time class
//*********************************************************

#include "Connect_Time.hpp"

#include <string.h>
#include <stdio.h>

//---------------------------------------------------------
//	Connect_Time constructor
//---------------------------------------------------------

Connect_Time::Connect_Time (void) : Connect_Data (0, 0), Lookup_Table (0, 0)
{
	Next_Rec (0);
	TOD_List (0);
	Dir_Index (0);
}

//---------------------------------------------------------
//	Connect_Time_Array constructor
//---------------------------------------------------------

Connect_Time_Array::Connect_Time_Array (int max_records) : 
	Connect_Array (max_records), Lookup_Size (0, 2)
{
	Data_Size (sizeof (Connect_Time));
}

void Connect_Time_Array::Clear (void)
{
	for (Connect_Time *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Connect_Array::Clear ();
}
