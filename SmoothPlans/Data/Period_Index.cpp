//*********************************************************
//	Period_Index.cpp - travel index by time period
//*********************************************************

#include "Period_Index.hpp"

//---------------------------------------------------------
//	Period_Index constructor
//---------------------------------------------------------

Period_Index::Period_Index (void)
{
	Period (0);
	Traveler (0);
	Original (0);
	Smoothed (0);
	Sum_Change (0);
	Max_Change (0);
	Min_Change (0);
}
void Period_Index::Add_Change (int value)
{
	original++;
	sum_change += value;
	if (value > max_change) max_change = value;
	if (value < min_change) min_change = value;
}

double Period_Index::Average_Change (void) 
{
	if (original > 0) {
		return ((double) sum_change / original);
	} else {
		return (0.0);
	}
}

//---------------------------------------------------------
//	Period_index_Array constructor
//---------------------------------------------------------

Period_Index_Array::Period_Index_Array (int max_record) : 
	Data_Array (sizeof (Period_Index), max_record)
{
}

void Period_Index_Array::Zero_Data (void)
{
	Period_Index *period_ptr;

	for (period_ptr = First (); period_ptr; period_ptr = Next ()) {
		period_ptr->Period (Record_Index ());
		period_ptr->Traveler (0);
		period_ptr->Original (0);
		period_ptr->Smoothed (0);
		period_ptr->Sum_Change (0);
		period_ptr->Max_Change (0);
		period_ptr->Min_Change (0);
	}
}
