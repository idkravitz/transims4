//*********************************************************
//	Point_Time_Data.hpp - point time data summary class
//*********************************************************

#ifndef POINT_TIME_DATA_HPP
#define POINT_TIME_DATA_HPP

#include "Class_Array.hpp"
#include "Class_Index.hpp"

#include "Lookup_Table.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Point_Time_Data class definition
//---------------------------------------------------------

class Point_Time_Data : public Class_Index, public Lookup_Table
{
public:
	Point_Time_Data (void);
	virtual ~Point_Time_Data (void)                       { Clear (); }

	int  Layers (void)                               { return (Columns ()); }
	int  Periods (void)                              { return (Rows ()); }

	int  In (int period)                             { return (Data (period, 1)); }
	int  Out (int period)                            { return (Data (period, 2)); }

	int  In2 (int period)                            { return (Data (period, 3)); }
	int  Out2 (int period)                           { return (Data (period, 4)); }

	void In (int period, int value)                  { Data (period, 1, value); }
	void Out (int period, int value)                 { Data (period, 2, value); }
	void Add_In (int period, int value)              { Add_Data (period, 1, value); }
	void Add_Out (int period, int value)             { Add_Data (period, 2, value); }

	void In2 (int period, int value)                 { Data (period, 3, value); }
	void Out2 (int period, int value)                { Data (period, 4, value); }
	void Add_In2 (int period, int value)             { Add_Data (period, 3, value); }
	void Add_Out2 (int period, int value)            { Add_Data (period, 4, value); }

	bool Periods (int periods, int layers = 2)       { return (Size (periods, layers)); }
};

//---------------------------------------------------------
//	Point_Time_Array class definition
//---------------------------------------------------------

class Point_Time_Array : public Class_Array, public Lookup_Size
{
public:
	Point_Time_Array (int max_records = 0);
	virtual ~Point_Time_Array (void)         { Class_Array::Clear (); }

	bool Add (Point_Time_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Point_Time_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Point_Time_Data *) Class_Array::New_Record (clear, number)); }

	Point_Time_Data * Get (int id)           { return ((Point_Time_Data *) Class_Array::Get (id)); }

	Point_Time_Data * Record (int index)     { return ((Point_Time_Data *) Class_Array::Record (index)); }
	Point_Time_Data * Record (void)          { return ((Point_Time_Data *) Class_Array::Record ()); }

	Point_Time_Data * First (void)           { return ((Point_Time_Data *) Class_Array::First ()); }
	Point_Time_Data * Next (void)            { return ((Point_Time_Data *) Class_Array::Next ()); }
	
	Point_Time_Data * Last (void)            { return ((Point_Time_Data *) Class_Array::Last ()); }
	Point_Time_Data * Previous (void)        { return ((Point_Time_Data *) Class_Array::Previous ()); }
	
	Point_Time_Data * First_Key (void)       { return ((Point_Time_Data *) Class_Array::First_Key ()); }
	Point_Time_Data * Next_Key (void)        { return ((Point_Time_Data *) Class_Array::Next_Key ()); }

	Point_Time_Data * operator[] (int index) { return (Record (index)); }

	virtual void Clear (void);

	int  Period (int tod)                    { return (Row (tod)); }
	void Time_Periods (Time_Range *range)    { Row_Range (range); }

	int  Period_Size (void)                  { return (Row_Size ()); }
	void Period_Size (int value)             { Row_Size (value); Rows ((MIDNIGHT + Row_Size () - 1) / Row_Size ()); }

	int  Periods (void)                      { return (Rows ()); }
	void Periods (int value)                 { Rows (value); }

	int  Layers (void)                       { return (Columns ()); }
	void Layers (int value)                  { Columns (value); }
};

#endif
