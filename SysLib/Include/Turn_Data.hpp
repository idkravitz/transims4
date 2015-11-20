//*********************************************************
//	Turn_Data.hpp - network turn penalty data
//*********************************************************

#ifndef TURN_DATA_HPP
#define TURN_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Turn_Data class definition
//---------------------------------------------------------

class Turn_Data : public Class2_Index
{
public:
	Turn_Data (int in_link_dir = 0, int out_link_dir = 0);
	virtual ~Turn_Data (void)      {}

	int  In_Link_Dir (void)        { return (Key1 ()); }
	int  In_Link (void)            { return ((Key1 () >> 1)); }
	int  In_Dir (void)             { return ((Key1 () & 1)); } 
	int  Out_Link_Dir (void)       { return (Key2 ()); }
	int  Out_Link (void)           { return ((Key2 () >> 1)); }
	int  Out_Dir (void)            { return ((Key2 () & 1)); } 
	int  Start (void)              { return (start); }
	int  End (void)                { return (end); }
	int  Use (void)                { return (use); }
	int  Penalty (void)            { return (penalty); }

	void In_Link_Dir (int value)   { Key1 (value); }
	void In_Link (int value)       { Key1 ((value << 1) + In_Dir ()); }
	void In_Dir (int value)        { Key1 ((In_Link () << 1) + value); }
	void Out_Link_Dir (int value)  { Key2 (value); }
	void Out_Link (int value)      { Key2 ((value << 1) + Out_Dir ()); }
	void Out_Dir (int value)       { Key2 ((Out_Link () << 1) + value); }
	void Start (int value)         { start = value; }
	void End (int value)           { end = value; }
	void Use (int value)           { use = (unsigned short) value; }
	void Penalty (int value)       { penalty = (unsigned short) value; }

private:
	int  start;		//---- rounded ----
	int  end;		//---- rounded ----
	unsigned short use;
	unsigned short penalty;
};

//---------------------------------------------------------
//	Turn_Array class definition
//---------------------------------------------------------

class Turn_Array : public Class2_Array
{
public:
	Turn_Array (int max_records = 0);

	bool Add (Turn_Data *data = NULL)      { return (Complex_Array::Add (data)); }
	
	Turn_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((Turn_Data *) Class2_Array::New_Record (clear, number)); }

	Turn_Data * Record (int index)         { return ((Turn_Data *) Class2_Array::Record (index)); }
	Turn_Data * Record (void)              { return ((Turn_Data *) Class2_Array::Record ()); }

	Turn_Data * Get (int key1, int key2)   { return ((Turn_Data *) Class2_Array::Get (key1, key2)); }
	Turn_Data * Get (Turn_Data *data)      { return ((Turn_Data *) Class2_Array::Get (data)); }

	Turn_Data * First (void)               { return ((Turn_Data *) Class2_Array::First ()); }
	Turn_Data * Next (void)                { return ((Turn_Data *) Class2_Array::Next ()); }
	
	Turn_Data * Last (void)                { return ((Turn_Data *) Class2_Array::Last ()); }
	Turn_Data * Previous (void)            { return ((Turn_Data *) Class2_Array::Previous ()); }

	Turn_Data * First_Key (void)           { return ((Turn_Data *) Class2_Array::First_Key ()); }
	Turn_Data * Next_Key (void)            { return ((Turn_Data *) Class2_Array::Next_Key ()); }

	Turn_Data * operator[] (int index)     { return (Record (index)); }
};

#endif
