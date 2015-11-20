//*********************************************************
//	Timing_Data.hpp - network timing plan data
//*********************************************************

#ifndef TIMING_DATA_HPP
#define TIMING_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Timing_Data class definition
//---------------------------------------------------------

class Timing_Data : public Class_Index
{
public:
	Timing_Data (void);
	virtual ~Timing_Data (void)     {}

	int  Plan_ID (void)            { return (ID ()); }
	int  Timing (void)             { return ((ID () >> 8)); }
	int  Phase (void)              { return ((ID () & 0xFF)); }
	int  Next_Phase (void)         { return (next); }
	int  Min_Green (void)          { return (minimum); }
	int  Max_Green (void)          { return (maximum); }
	int  Ext_Green (void)          { return (extend); }
	int  Yellow (void)             { return (yellow); }
	int  Red_Clear (void)          { return (red); }
	int  Ring (void)               { return (ring); }
	int  Group (void)              { return (group); }

	void Plan_ID (int value)       { ID (value); }
	void Timing (int value)        { ID ((value << 8) + Phase ()); }
	void Phase (int value)         { ID ((Timing () << 8) + value); }
	void Next_Phase (int value)    { next = (short) value; }
	void Min_Green (int value)     { minimum = (short) value; }
	void Max_Green (int value)     { maximum = (short) value; }
	void Ext_Green (int value)     { extend = (short) value; }
	void Yellow (int value)        { yellow = (char) value; }
	void Red_Clear (int value)     { red = (char) value; }
	void Ring (int value)          { ring = (char) value; }
	void Group (int value)         { group = (char) value; }

private:
	short next;
	short minimum;
	short maximum;
	short extend;
	char  yellow;
	char  red;
	char  ring;
	char  group;
};

//---------------------------------------------------------
//	Timing_Array class definition
//---------------------------------------------------------

class Timing_Array : public Class_Array
{
public:
	Timing_Array (int max_records = 0);

	bool Add (Timing_Data *data = NULL)      { return (Class_Array::Add (data)); }
	
	Timing_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Timing_Data *) Class_Array::New_Record (clear, number)); }

	Timing_Data * Record (int index)         { return ((Timing_Data *) Class_Array::Record (index)); }
	Timing_Data * Record (void)              { return ((Timing_Data *) Class_Array::Record ()); }

	Timing_Data * Get (Timing_Data *data)    { return ((Timing_Data *) Class_Array::Get (data)); }
	Timing_Data * Get_GE (Timing_Data *data) { return ((Timing_Data *) Class_Array::Get_GE (data)); }
	Timing_Data * Get_LE (Timing_Data *data) { return ((Timing_Data *) Class_Array::Get_LE (data)); }

	Timing_Data * First (void)               { return ((Timing_Data *) Class_Array::First ()); }
	Timing_Data * Next (void)                { return ((Timing_Data *) Class_Array::Next ()); }
	
	Timing_Data * Last (void)                { return ((Timing_Data *) Class_Array::Last ()); }
	Timing_Data * Previous (void)            { return ((Timing_Data *) Class_Array::Previous ()); }

	Timing_Data * First_Key (void)           { return ((Timing_Data *) Class_Array::First_Key ()); }
	Timing_Data * Next_Key (void)            { return ((Timing_Data *) Class_Array::Next_Key ()); }

	Timing_Data * operator[] (int index)     { return (Record (index)); }
};

#endif
