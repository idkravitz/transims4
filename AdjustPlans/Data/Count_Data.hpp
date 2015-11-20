//*********************************************************
//	Count_Data.hpp - traffic count data classes
//*********************************************************

#ifndef COUNT_DATA_HPP
#define COUNT_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Count_Data class definition
//---------------------------------------------------------

class Count_Data : public Class2_Index
{
public:
	Count_Data (int link_dir = 0, int time = 0);

	int  Dir_Index (void)               { return (Key1 ()); }
	int  Start_Time (void)              { return (Key2 ()); }
	int  End_Time (void)                { return (end); }
	int  Count (void)                   { return (count); }
	int  Volume (void)                  { return (volume); }
	int  Difference (void)              { return (volume - count); }

	void Dir_Index (int value)          { Key1 (value); }
	void Start_Time (int value)         { Key2 (value); }
	void End_Time (int value)           { end = value; }
	void Count (int value)              { count = value; }
	void Volume (int value)             { volume = value; }

	void Add_Count (int value = 1)		{ count += value; }
	void Add_Volume (int value = 1)     { volume += value; }
	
private:
	int  end;			//---- rounded ----
	int  count;
	int  volume;
};

//---------------------------------------------------------
//	Count_Array class definition
//---------------------------------------------------------

class Count_Array : public Class2_Array
{
public:
	Count_Array (int max_records = 0);

	bool Add (Count_Data *data = NULL)       { return (Class2_Array::Add (data)); }
	
	Count_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Count_Data *) Class2_Array::New_Record (clear, number)); }

	Count_Data * Record (int index)          { return ((Count_Data *) Class2_Array::Record (index)); }
	Count_Data * Record (void)               { return ((Count_Data *) Class2_Array::Record ()); }

	Count_Data * Get (int key1, int key2)    { return ((Count_Data *) Class2_Array::Get (key1, key2)); }
	Count_Data * Get (Count_Data *data)      { return ((Count_Data *) Class2_Array::Get (data)); }

	Count_Data * Get_GE (int key1, int key2) { return ((Count_Data *) Class2_Array::Get_GE (key1, key2)); }
	Count_Data * Get_GE (Count_Data *data)   { return ((Count_Data *) Class2_Array::Get_GE (data)); }

	Count_Data * Get_LE (int key1, int key2) { return ((Count_Data *) Class2_Array::Get_LE (key1, key2)); }
	Count_Data * Get_LE (Count_Data *data)   { return ((Count_Data *) Class2_Array::Get_LE (data)); }

	Count_Data * First (void)                { return ((Count_Data *) Class2_Array::First ()); }
	Count_Data * Next (void)                 { return ((Count_Data *) Class2_Array::Next ()); }
	
	Count_Data * Last (void)                 { return ((Count_Data *) Class2_Array::Last ()); }
	Count_Data * Previous (void)             { return ((Count_Data *) Class2_Array::Previous ()); }

	Count_Data * First_Key (void)            { return ((Count_Data *) Class2_Array::First_Key ()); }
	Count_Data * Next_Key (void)             { return ((Count_Data *) Class2_Array::Next_Key ()); }

	Count_Data * Last_Key (void)             { return ((Count_Data *) Class2_Array::Last_Key ()); }
	Count_Data * Previous_Key (void)         { return ((Count_Data *) Class2_Array::Previous_Key ()); }

	Count_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
