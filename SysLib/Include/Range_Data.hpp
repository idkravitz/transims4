//*********************************************************
//	Range_Data.hpp - range data classes
//*********************************************************

#ifndef RANGE_DATA_HPP
#define RANGE_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Range_Data class definition
//---------------------------------------------------------

class Range_Data
{
public:
	Range_Data (void);

	int  Low (void)           { return (low); }
	int  High (void)          { return (high); }

	void Low (int value)      { low = value; }
	void High (int value)     { high = value; }

private:
	int  low;
	int  high;
};

//---------------------------------------------------------
//	Range_Array
//---------------------------------------------------------

class Range_Array : public Data_Array
{
public:
	Range_Array (int max_records = 0);

	bool In_Range (int value);
	bool Span_Range (int low, int high);
	bool Span_Range (Range_Data *range)	{ return (Span_Range (range->Low (), range->High ())); }

	bool Add_Range (int low, int high, int increment = 0);

	int  Max_Value (void);
	int  Max_Count (void);
	int  Range_Count (int value);

	int  In_Index (int value)           { return ((In_Range (value)) ? Record_Index () : 0); }
	int  Span_Index (int low, int high) { return ((Span_Range (low, high)) ? Record_Index () : 0); }

	int  Num_Ranges (void)              { return (Num_Records ()); };

	bool Add (Range_Data *data)         { return (Data_Array::Add ((void *) data)); }

	Range_Data * First (void)           { return ((Range_Data *) Data_Array::First ()); }
	Range_Data * Next (void)            { return ((Range_Data *) Data_Array::Next ()); }

	Range_Data * Last (void)            { return ((Range_Data *) Data_Array::Last ()); }
	Range_Data * Previous (void)        { return ((Range_Data *) Data_Array::Previous ()); }

	Range_Data * operator[] (int index) { return ((Range_Data *) Record (index)); }
};

#endif
