//*********************************************************
//	TTime_Data.hpp - network travel time class
//*********************************************************

#ifndef TTIME_DATA_HPP
#define TTIME_DATA_HPP

#include "Dir_Data.hpp"

#include "Lookup_Table.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	TTime_Data class definition
//---------------------------------------------------------

class TTime_Data : public Dir_Data, public Lookup_Table
{
public:
	TTime_Data (void);
	virtual ~TTime_Data (void)                       { Clear (); }

	int  Layers (void)                               { return (Columns ()); }
	int  Periods (void)                              { return (Rows ()); }

	int  TTime (int period)                          { int t = Data (period, 1); return ((t > 0) ? t : Time0 ()); }
	int  Volume (int period)                         { return (Data (period, 2)); }

	int  TTime2 (int period)                         { int t = Data (period, 3); return ((t > 0) ? t : Time0 ()); }
	int  Volume2 (int period)                        { return (Data (period, 4)); }

	void TTime (int period, int value)               { Data (period, 1, value); }
	void Volume (int period, int value)              { Data (period, 2, value); }
	void Add_TTime (int period, int value)           { Add_Data (period, 1, value); }
	void Add_Volume (int period, int value)          { Add_Data (period, 2, value); }

	void TTime2 (int period, int value)              { Data (period, 3, value); }
	void Volume2 (int period, int value)             { Data (period, 4, value); }
	void Add_TTime2 (int period, int value)          { Add_Data (period, 3, value); }
	void Add_Volume2 (int period, int value)         { Add_Data (period, 4, value); }

	bool Periods (int periods, int layers = 2)       { return (Size (periods, layers)); }

	//---- overloaded methods ----

	int  Count (int period)                          { return (Data (period, 1)); }
	void Count (int period, int value)               { Data (period, 1, value); }
	void Add_Count (int period, int value)           { Add_Data (period, 1, value); }
};


//---------------------------------------------------------
//	TTime_Array class definition
//---------------------------------------------------------

class TTime_Array : public Dir_Array, public Lookup_Size
{
public:
	TTime_Array (int max_records = 0);
	virtual ~TTime_Array (void)              { TTime_Array::Clear (); }

	bool Add (TTime_Data *data = NULL)       { return (Dir_Array::Add (data)); }
	
	TTime_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((TTime_Data *) Dir_Array::New_Record (clear, number)); }

	TTime_Data * Get (int link)              { return ((TTime_Data *) Dir_Array::Get (link)); }
	TTime_Data * Get_GE (int link)           { return ((TTime_Data *) Dir_Array::Get_GE (link)); }
	TTime_Data * Get_LE (int link)           { return ((TTime_Data *) Dir_Array::Get_LE (link)); }

	TTime_Data * Record (int index)          { return ((TTime_Data *) Dir_Array::Record (index)); }
	TTime_Data * Record (void)               { return ((TTime_Data *) Dir_Array::Record ()); }

	TTime_Data * First (void)                { return ((TTime_Data *) Dir_Array::First ()); }
	TTime_Data * Next (void)                 { return ((TTime_Data *) Dir_Array::Next ()); }
	
	TTime_Data * Last (void)                 { return ((TTime_Data *) Dir_Array::Last ()); }
	TTime_Data * Previous (void)             { return ((TTime_Data *) Dir_Array::Previous ()); }
	
	TTime_Data * First_Key (void)            { return ((TTime_Data *) Dir_Array::First_Key ()); }
	TTime_Data * Next_Key (void)             { return ((TTime_Data *) Dir_Array::Next_Key ()); }

	TTime_Data * operator[] (int index)      { return (Record (index)); }

	virtual void Clear (void);

	int  Period (int tod)                    { return (Row (tod)); }
	void Time_Periods (Time_Range *range)    { Row_Range (range); }

	int  Period_Size (void)                  { return (Row_Size ()); }
	bool Period_Size (int size, int min_size = 0, int max_size = MIDNIGHT, bool wrap_flag = true) 
	                                         { return (Row_Size (size, min_size, max_size, wrap_flag)); }

	int  Periods (void)                      { return (Rows ()); }
	void Periods (int value)                 { Rows (value); }

	int  Layers (void)                       { return (Columns ()); }
	void Layers (int value)                  { Columns (value); }
};

#endif
