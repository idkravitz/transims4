//*********************************************************
//	Connect_Time.hpp - connection time class
//*********************************************************

#ifndef CONNECT_TIME_HPP
#define CONNECT_TIME_HPP

#include "Connect_Data.hpp"

#include "Lookup_Table.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Connect_Time class definition
//---------------------------------------------------------

class Connect_Time : public Connect_Data, public Lookup_Table
{
public:
	Connect_Time (void);
	virtual ~Connect_Time (void)                     { Clear (); }

	int  Next_Rec (void)                             { return (next_rec); }
	int  TOD_List (void)                             { return (tod_list); }
	int  Dir_Index (void)                            { return (dir_index); } 

	int  Layers (void)                               { return (Columns ()); }
	int  Periods (void)                              { return (Rows ()); }

	int  TTime (int period)                          { return (Data (period, 1)); }
	int  Volume (int period)                         { return (Data (period, 2)); }

	int  TTime2 (int period)                         { return (Data (period, 3)); }
	int  Volume2 (int period)                        { return (Data (period, 4)); }

	void Next_Rec (int value)                        { next_rec = value; }
	void TOD_List (int value)                        { tod_list = value; }
	void Dir_Index (int value)                       { dir_index = value; }

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

private:
	int   next_rec;
	int   tod_list;
	int   dir_index;
};


//---------------------------------------------------------
//	Connect_Time_Array class definition
//---------------------------------------------------------

class Connect_Time_Array : public Connect_Array, public Lookup_Size
{
public:
	Connect_Time_Array (int max_records = 0);
	virtual ~Connect_Time_Array (void)         { Connect_Time_Array::Clear (); }

	bool Add (Connect_Time *data = NULL)       { return (Connect_Array::Add (data)); }
	
	Connect_Time * New_Record (bool clear = false, int number = 1)
	                                           { return ((Connect_Time *) Connect_Array::New_Record (clear, number)); }
	
	Connect_Time * Get (int key1, int key2)    { return ((Connect_Time *) Connect_Array::Get (key1, key2)); }
	Connect_Time * Get (Connect_Time *data)    { return ((Connect_Time *) Connect_Array::Get (data)); }

	Connect_Time * Get_GE (int key1, int key2) { return ((Connect_Time *) Connect_Array::Get_GE (key1, key2)); }
	Connect_Time * Get_GE (Connect_Time *data) { return ((Connect_Time *) Connect_Array::Get_GE (data)); }

	Connect_Time * Get_LE (int key1, int key2) { return ((Connect_Time *) Class2_Array::Get_LE (key1, key2)); }
	Connect_Time * Get_LE (Connect_Time *data) { return ((Connect_Time *) Class2_Array::Get_LE (data)); }

	Connect_Time * Record (int index)          { return ((Connect_Time *) Connect_Array::Record (index)); }
	Connect_Time * Record (void)               { return ((Connect_Time *) Connect_Array::Record ()); }

	Connect_Time * First (void)                { return ((Connect_Time *) Connect_Array::First ()); }
	Connect_Time * Next (void)                 { return ((Connect_Time *) Connect_Array::Next ()); }
	
	Connect_Time * Last (void)                 { return ((Connect_Time *) Connect_Array::Last ()); }
	Connect_Time * Previous (void)             { return ((Connect_Time *) Connect_Array::Previous ()); }
	
	Connect_Time * First_Key (void)            { return ((Connect_Time *) Connect_Array::First_Key ()); }
	Connect_Time * Next_Key (void)             { return ((Connect_Time *) Connect_Array::Next_Key ()); }

	Connect_Time * operator[] (int index)      { return (Record (index)); }

	virtual void Clear (void);

	int  Period (int tod)                      { return (Row (tod)); }
	void Time_Periods (Time_Range *range)      { Row_Range (range); }

	int  Period_Size (void)                    { return (Row_Size ()); }
	bool Period_Size (int size, int min_size = 0, int max_size = MIDNIGHT, bool wrap_flag = true) 
	                                           { return (Row_Size (size, min_size, max_size, wrap_flag)); }

	int  Periods (void)                        { return (Rows ()); }
	void Periods (int value)                   { Rows (value); }

	int  Layers (void)                         { return (Columns ()); }
	void Layers (int value)                    { Columns (value); }
};
#endif
