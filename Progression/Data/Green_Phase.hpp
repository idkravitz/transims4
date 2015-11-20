//*********************************************************
//	Green_Phase.hpp - signal green phase movements data
//*********************************************************

#ifndef GREEN_PHASE_HPP
#define GREEN_PHASE_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Green_Phase class definition
//---------------------------------------------------------

class Green_Phase : public Class2_Index
{
public:
	Green_Phase (int in_link_dir = 0, int out_link_dir = 0);
	virtual ~Green_Phase (void)     {}

	int  In_Link_Dir (void)         { return (Key1 ()); }
	int  In_Link (void)             { return ((Key1 () >> 1)); }
	int  In_Dir (void)              { return ((Key1 () & 1)); } 
	int  Out_Link_Dir (void)        { return (Key2 ()); }
	int  Out_Link (void)            { return ((Key2 () >> 1)); }
	int  Out_Dir (void)             { return ((Key2 () & 1)); } 
	int  Green_Time (void)          { return (green); }
	int  Phase_Offset (void)        { return (offset); }
	int  Movement (void)            { return (movement); }

	void In_Link_Dir (int value)    { Key1 (value); }
	void In_Link (int value)        { Key1 ((value << 1) + In_Dir ()); }
	void In_Dir (int value)         { Key1 ((In_Link () << 1) + value); }
	void Out_Link_Dir (int value)   { Key2 (value); }
	void Out_Link (int value)       { Key2 ((value << 1) + Out_Dir ()); }
	void Out_Dir (int value)        { Key2 ((Out_Link () << 1) + value); }
	void Green_Time (int value)     { green = value; }
	void Phase_Offset (int value)   { offset = value; }
	void Movement (int value)       { movement = value; }

	//---- overloaded methods ----

	int  Protected (void)           { return (green); }
	int  Permitted (void)           { return (offset); }

	void Protected (int phase)      { green = phase; }
	void Permitted (int phase)      { offset = phase; }

private:
	int green;
	int offset;
	int movement;
};

//---------------------------------------------------------
//	Green_Phase_Array class definition
//---------------------------------------------------------

class Green_Phase_Array : public Class2_Array
{
public:
	Green_Phase_Array (int max_records = 0);

	bool Add (Green_Phase *data = NULL)     { return (Class2_Array::Add (data)); }
	
	Green_Phase * New_Record (bool clear = false, int number = 1)
	                                        { return ((Green_Phase *) Class2_Array::New_Record (clear, number)); }

	Green_Phase * Record (int index)        { return ((Green_Phase *) Class2_Array::Record (index)); }
	Green_Phase * Record (void)             { return ((Green_Phase *) Class2_Array::Record ()); }

	Green_Phase * Get (int key1, int key2)  { return ((Green_Phase *) Class2_Array::Get (key1, key2)); }
	Green_Phase * Get (Green_Phase *data)   { return ((Green_Phase *) Class2_Array::Get (data)); }

	Green_Phase * First (void)              { return ((Green_Phase *) Class2_Array::First ()); }
	Green_Phase * Next (void)               { return ((Green_Phase *) Class2_Array::Next ()); }

	Green_Phase * First_Key (void)          { return ((Green_Phase *) Class2_Array::First_Key ()); }
	Green_Phase * Next_Key (void)           { return ((Green_Phase *) Class2_Array::Next_Key ()); }

	Green_Phase * operator[] (int index)    { return (Record (index)); }
};

#endif
