//*********************************************************
//	Range_Data.cpp - range data classes
//*********************************************************

#include "Range_Data.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Range_Data constructor
//---------------------------------------------------------

Range_Data::Range_Data (void)
{
	Low (0);
	High (MAX_INTEGER);
}

//---------------------------------------------------------
//	Range_Array constructor
//---------------------------------------------------------

Range_Array::Range_Array (int max_records) : 
	Data_Array (sizeof (Range_Data), max_records)
{
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Range_Array::In_Range (int value)
{
	for (Range_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (value >= ptr->Low () && value <= ptr->High ()) return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Span_Range
//---------------------------------------------------------

bool Range_Array::Span_Range (int low, int high)
{
	for (Range_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (high >= ptr->Low () && low <= ptr->High ()) return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Range_Count
//---------------------------------------------------------

int Range_Array::Range_Count (int value)
{
	int count = 0;

	for (Range_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (value >= ptr->Low () && value <= ptr->High ()) {
			count += value - ptr->Low () + 1;
			return (count);
		} else {
			count += ptr->High () - ptr->Low () + 1;
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Max_Value
//---------------------------------------------------------

int Range_Array::Max_Value (void)
{
	int max = 0;

	for (Range_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		if (ptr->High () > max) max = ptr->High ();
	}
	return (max);
}

//---------------------------------------------------------
//	Max_Count
//---------------------------------------------------------

int Range_Array::Max_Count (void)
{
	int count = 0;

	for (Range_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		count += ptr->High () - ptr->Low () + 1;
	}
	return (count);
}

//---------------------------------------------------------
//	Add_Range
//---------------------------------------------------------

bool Range_Array::Add_Range (int low, int high, int increment)
{
	Range_Data range;

	if (increment > 0 && (low + increment) <= high) {
		for (int inc = low; inc <= high;) { 
			range.Low (inc);
			inc += increment;
			range.High (inc - 1);

			if (!Add (&range)) return (false);
		}
		return (true);
	}
	range.Low (low);
	range.High (high);

	return (Add (&range));
}
