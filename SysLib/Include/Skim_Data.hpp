//*********************************************************
//	Skim_Data.hpp - skim data classes
//*********************************************************

#ifndef SKIM_DATA_HPP
#define SKIM_DATA_HPP

#include "Class_Array.hpp"
#include "Matrix_Data.hpp"

//---------------------------------------------------------
//	Skim_Data class definition
//---------------------------------------------------------

class Skim_Data : public Class_Index
{
public:
	Skim_Data (int key = 0);

	int  Count (void)                 { return (count); }
	int  Walk (void)                  { return (walk); }
	int  Wait (void)                  { return (wait); }
	int  Transit (void)               { return (transit); }
	int  Drive (void)                 { return (drive); }
	int  Other (void)                 { return (other); }
	int  Length (void)                { return (length); }
	int  Cost (void)                  { return (cost); }

	void Count (int value)            { count = value; }
	void Walk (int value)             { walk = value; }
	void Wait (int value)             { wait = value; }
	void Transit (int value)          { transit = value; }
	void Drive (int value)            { drive = value; }
	void Other (int value)            { other = value; }
	void Length (int value)           { length = value; }
	void Cost (int value)             { cost = value; }

	void Add_Count (int value)        { count += value; }
	void Add_Walk (int value)         { walk += value; }
	void Add_Wait (int value)         { wait += value; }
	void Add_Transit (int value)      { transit += value; }
	void Add_Drive (int value)        { drive += value; }
	void Add_Other (int value)        { other += value; }
	void Add_Length (int value)       { length += value; }
	void Add_Cost (int value)         { cost += value; }

	void Add_Skim (Skim_Data *skim);
	void New_Skim (void);

private:
	int count;
	int walk;
	int wait;
	int transit;
	int drive;
	int other;
	int length;
	int cost;
};

//---------------------------------------------------------
//	Skim_Array class definition
//---------------------------------------------------------

class Skim_Array : public Class_Array, public Matrix_Key
{
public:
	Skim_Array (int num_periods = 1, int num_zones = 0, int max_records = 0);

	bool Add (Skim_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Skim_Data * New_Record (bool clear = false, int number = 1)
	                                   { return ((Skim_Data *) Class_Array::New_Record (clear, number)); }

	Skim_Data * Record (int index)     { return ((Skim_Data *) Class_Array::Record (index)); }
	Skim_Data * Record (void)          { return ((Skim_Data *) Class_Array::Record ()); }

	Skim_Data * Get (int key)          { return ((Skim_Data *) Class_Array::Get (key)); }
	Skim_Data * Get (Skim_Data *key)   { return ((Skim_Data *) Class_Array::Get (key)); }

	Skim_Data * Get (int org, int des, int period = 1) { return (Get (Key (org, des, period))); }

	Skim_Data * First (void)           { return ((Skim_Data *) Class_Array::First ()); }
	Skim_Data * Next (void)            { return ((Skim_Data *) Class_Array::Next ()); }
	
	Skim_Data * Last (void)            { return ((Skim_Data *) Class_Array::Last ()); }
	Skim_Data * Previous (void)        { return ((Skim_Data *) Class_Array::Previous ()); }

	Skim_Data * First_Key (void)       { return ((Skim_Data *) Class_Array::First_Key ()); }
	Skim_Data * Next_Key (void)        { return ((Skim_Data *) Class_Array::Next_Key ()); }

	Skim_Data * operator[] (int index) { return (Record (index)); }
};

#endif
