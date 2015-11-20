//*********************************************************
//	Control_Data.hpp - traffic control data classes
//*********************************************************

#ifndef CONTROL_DATA_HPP
#define CONTROL_DATA_HPP

#include "Class_Array.hpp"

#define MAX_CONTROL_CONFLICTS	2

//---------------------------------------------------------
//	Control_Data class definition
//---------------------------------------------------------

class Control_Data : public Class2_Index
{
public:
	Control_Data (int in_link_dir = 0, int out_link_dir = 0);

	int  In_Link_Dir (void)                { return (Key1 ()); }
	int  In_Link (void)                    { return ((Key1 () >> 1)); }
	int  In_Dir (void)                     { return ((Key1 () & 1)); } 
	int  Out_Link_Dir (void)               { return (Key2 ()); }
	int  Out_Link (void)                   { return ((Key2 () >> 1)); }
	int  Out_Dir (void)                    { return ((Key2 () & 1)); } 
	int  Control_Status (void)             { return (control_status); }
	int  Approach_Dir (void)               { return (approach_dir); }
	int  Conflict_Dir (int num)            { return (Index (num) ? conflict_dir [num-1] : 0); }
	int  Low_Lane (int num)                { return (Index (num) ? low_lane [num-1] : 0); }
	int  High_Lane (int num)               { return (Index (num) ? high_lane [num-1] : 0); }

	void In_Link_Dir (int value)           { Key1 (value); }
	void In_Link (int value)               { Key1 ((value << 1) + In_Dir ()); }
	void In_Dir (int value)                { Key1 ((In_Link () << 1) + value); }
	void Out_Link_Dir (int value)          { Key2 (value); }
	void Out_Link (int value)              { Key2 ((value << 1) + Out_Dir ()); }
	void Out_Dir (int value)               { Key2 ((Out_Link () << 1) + value); }
	void Control_Status (int value)        { control_status = value; }
	void Approach_Dir (int value)          { approach_dir = value; }
	void Conflict_Dir (int num, int value) { if (Index (num)) conflict_dir [num-1] = value; }
	void Low_Lane (int num, int value)     { if (Index (num)) low_lane [num-1] = (char) value; }
	void High_Lane (int num, int value)    { if (Index (num)) high_lane [num-1] = (char) value; }

	int  Max_Conflicts (void)              { return (MAX_CONTROL_CONFLICTS); }

private:
	int  control_status;
	int  approach_dir;
	int  conflict_dir [MAX_CONTROL_CONFLICTS];
	char low_lane [MAX_CONTROL_CONFLICTS];
	char high_lane [MAX_CONTROL_CONFLICTS];

	bool Index (int num)                   { return (num > 0 && num <= Max_Conflicts ()); }
};

//---------------------------------------------------------
//	Control_Array class definition
//---------------------------------------------------------

class Control_Array : public Class2_Array
{
public:
	Control_Array (int max_records = 0);

	bool Add (Control_Data *data = NULL)       { return (Class2_Array::Add (data)); }
	
	Control_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Control_Data *) Class2_Array::New_Record (clear, number)); }

	Control_Data * Record (int index)          { return ((Control_Data *) Class2_Array::Record (index)); }
	Control_Data * Record (void)               { return ((Control_Data *) Class2_Array::Record ()); }

	Control_Data * Get (int key1, int key2)    { return ((Control_Data *) Class2_Array::Get (key1, key2)); }
	Control_Data * Get (Control_Data *data)    { return ((Control_Data *) Class2_Array::Get (data)); }

	Control_Data * Get_GE (int key1, int key2) { return ((Control_Data *) Class2_Array::Get_GE (key1, key2)); }
	Control_Data * Get_GE (Control_Data *data) { return ((Control_Data *) Class2_Array::Get_GE (data)); }

	Control_Data * Get_LE (int key1, int key2) { return ((Control_Data *) Class2_Array::Get_LE (key1, key2)); }
	Control_Data * Get_LE (Control_Data *data) { return ((Control_Data *) Class2_Array::Get_LE (data)); }

	Control_Data * First (void)                { return ((Control_Data *) Class2_Array::First ()); }
	Control_Data * Next (void)                 { return ((Control_Data *) Class2_Array::Next ()); }
	
	Control_Data * Last (void)                 { return ((Control_Data *) Class2_Array::Last ()); }
	Control_Data * Previous (void)             { return ((Control_Data *) Class2_Array::Previous ()); }

	Control_Data * First_Key (void)            { return ((Control_Data *) Class2_Array::First_Key ()); }
	Control_Data * Next_Key (void)             { return ((Control_Data *) Class2_Array::Next_Key ()); }

	Control_Data * Last_Key (void)             { return ((Control_Data *) Class2_Array::Last_Key ()); }
	Control_Data * Previous_Key (void)         { return ((Control_Data *) Class2_Array::Previous_Key ()); }

	Control_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
