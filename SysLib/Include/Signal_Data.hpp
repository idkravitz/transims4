//*********************************************************
//	Signal_Data.hpp - network signalized node data
//*********************************************************

#ifndef SIGNAL_DATA_HPP
#define SIGNAL_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Signal_Data class definition
//---------------------------------------------------------

class Signal_Data : public Class2_Index
{
public:
	Signal_Data (int node = 0, int start = 0);
	virtual ~Signal_Data (void) {}

	int  Node (void)               { return (Key1 ()); }
	int  Start (void)              { return (Key2 ()); }
	int  End (void)                { return (end); }
	int  Timing (void)             { return (timing); }
	int  Type (void)               { return (type); }
	int  Rings (void)              { return (rings); }
	int  Groups (void)             { return (groups); }
	int  Offset (void)             { return (offset); }
	int  Coordinator (void)        { return (coordinator); }

	void Node (int value)          { Key1 (value); }
	void Start (int value)         { Key2 (value); }
	void End (int value)           { end = value; }
	void Timing (int value)        { timing = value; }
	void Type (int value)          { type = (short) value; }
	void Rings (int value)         { rings = (short) value; }
	void Groups (int value)        { groups = (short) value; }
	void Offset (int value)        { offset = (short) value; }
	void Coordinator (int value)   { coordinator = value; }

private:
	int   end;
	int   timing;
	short type;
	short rings;
	short groups;
	short offset;		//---- NOT rounded ----
	int   coordinator;
};

//---------------------------------------------------------
//	Signal_Array class definition
//---------------------------------------------------------

class Signal_Array : public Class2_Array
{
public:
	Signal_Array (int max_records = 0);

	bool Add (Signal_Data *data = NULL)        { return (Class2_Array::Add (data)); }
	
	Signal_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Signal_Data *) Class2_Array::New_Record (clear, number)); }

	Signal_Data * Record (int index)           { return ((Signal_Data *) Class2_Array::Record (index)); }
	Signal_Data * Record (void)                { return ((Signal_Data *) Class2_Array::Record ()); }

	Signal_Data * Get (int node, int start)    { return ((Signal_Data *) Class2_Array::Get (node, start)); }
	Signal_Data * Get (Signal_Data *data)      { return ((Signal_Data *) Class2_Array::Get (data)); }

	Signal_Data * Get_GE (int node, int start) { return ((Signal_Data *) Class2_Array::Get_GE (node, start)); }
	Signal_Data * Get_GE (Signal_Data *data)   { return ((Signal_Data *) Class2_Array::Get_GE (data)); }

	Signal_Data * Get_LE (int node, int start) { return ((Signal_Data *) Class2_Array::Get_LE (node, start)); }
	Signal_Data * Get_LE (Signal_Data *data)   { return ((Signal_Data *) Class2_Array::Get_LE (data)); }

	Signal_Data * First (void)                 { return ((Signal_Data *) Class2_Array::First ()); }
	Signal_Data * Next (void)                  { return ((Signal_Data *) Class2_Array::Next ()); }
	
	Signal_Data * Last (void)                  { return ((Signal_Data *) Class2_Array::Last ()); }
	Signal_Data * Previous (void)              { return ((Signal_Data *) Class2_Array::Previous ()); }

	Signal_Data * First_Key (void)             { return ((Signal_Data *) Class2_Array::First_Key ()); }
	Signal_Data * Next_Key (void)              { return ((Signal_Data *) Class2_Array::Next_Key ()); }

	Signal_Data * operator[] (int index)       { return (Record (index)); }
};

#endif
