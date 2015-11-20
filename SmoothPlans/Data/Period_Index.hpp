//*********************************************************
//	Period_Index.hpp - travel index by time period
//*********************************************************

#ifndef PERIOD_INDEX_HPP
#define PERIOD_INDEX_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Period_Index class definition
//---------------------------------------------------------

class Period_Index 
{
public:
	Period_Index (void);

	int  Period (void)                { return (period); }
	int  Traveler (void)              { return (traveler); }
	int  Original (void)              { return (original); }
	int  Smoothed (void)              { return (smoothed); }
	int  Sum_Change (void)            { return (sum_change); }
	int  Max_Change (void)            { return (max_change); }
	int  Min_Change (void)            { return (min_change); }

	void Period (int value)           { period = value; }
	void Traveler (int value)         { traveler = value; }
	void Original (int value)         { original = value; }
	void Smoothed (int value)         { smoothed = value; }
	void Sum_Change (int value)       { sum_change = value; }
	void Max_Change (int value)       { max_change = value; }
	void Min_Change (int value)       { min_change = value; }

	void Add_Traveler (int value)     { traveler = value; smoothed++; }

	void Add_Original (void)          { original++; }
	void Add_Smoothed (void)          { smoothed++; }

	void Add_Change (int value);
	double Average_Change (void);
	
private:
	int period;
	int traveler;
	int smoothed;
	int original;
	int sum_change;
	int max_change;
	int min_change;
};

//---------------------------------------------------------
//	Period_Index_Array class definition
//---------------------------------------------------------

class Period_Index_Array : public Data_Array
{
public:
	Period_Index_Array (int max_record = 0);

	bool Add (Period_Index *data = NULL)    { return (Data_Array::Add (data)); }
	
	Period_Index * New_Record (bool clear = false, int number = 1)
	                                        { return ((Period_Index *) Data_Array::New_Record (clear, number)); }

	Period_Index * Record (int index)       { return ((Period_Index *) Data_Array::Record (index)); }
	Period_Index * Record (void)            { return ((Period_Index *) Data_Array::Record ()); }

	Period_Index * First (void)             { return ((Period_Index *) Data_Array::First ()); }
	Period_Index * Next (void)              { return ((Period_Index *) Data_Array::Next ()); }
	
	Period_Index * Last (void)              { return ((Period_Index *) Data_Array::Last ()); }
	Period_Index * Previous (void)          { return ((Period_Index *) Data_Array::Previous ()); }

	Period_Index * operator[] (int index)   { return (Record (index)); }

	void Zero_Data (void);
};

#endif
