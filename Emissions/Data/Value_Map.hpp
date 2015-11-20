//*********************************************************
//	Value_Map.hpp - summary value map classes
//*********************************************************

#ifndef VALUE_MAP_HPP
#define VALUE_MAP_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Value_Map class definition
//---------------------------------------------------------

class Value_Map : public Class2_Index
{
public:
	Value_Map (int value = 0, int member = 0);

	int    Value (void)              { return (Key1 ()); }
	int    Group (void)              { return (Key2 () >> 16); }
	int    Member (void)             { return (Key2 () & 0x00FF); }
	double Weight (void)             { return (weight); } 

	void   Value (int value)         { Key1 (value); }
	void   Group (int value)         { Key2 ((value << 16) + Member ()); }
	void   Member (int value)        { Key2 ((Group () << 16) + value); }
	void   Weight (double value)     { weight = value; }

private:
	double weight;
};

//---------------------------------------------------------
//	Value_Map_Array class definition
//---------------------------------------------------------

class Value_Map_Array : public Class2_Array
{
public:
	Value_Map_Array (int max_records = 10);

	bool Add (Value_Map *data = NULL)       { return (Class2_Array::Add (data)); }

	int Get_Index (int key1, int key2)      { Value_Map key (key1, key2); return (Get_Index (&key)); }
	int Get_Index (Value_Map *data)         { return (Class2_Array::Get_Index (data)); }	

	Value_Map * Record (int index)          { return ((Value_Map *) Class2_Array::Record (index)); }
	Value_Map * Record (void)               { return ((Value_Map *) Class2_Array::Record ()); }

	Value_Map * Get (int key1, int key2)    { Value_Map key (key1, key2); return (Get (&key)); }
	Value_Map * Get (Value_Map *data)       { return ((Value_Map *) Class2_Array::Get (data)); }

	Value_Map * Get_GE (int key1, int key2) { Value_Map key (key1, key2); return (Get_GE (&key)); }
	Value_Map * Get_GE (Value_Map *data)    { return ((Value_Map *) Class2_Array::Get_GE (data)); }

	Value_Map * Get_LE (int key1, int key2) { Value_Map key (key1, key2); return (Get_LE (&key)); }
	Value_Map * Get_LE (Value_Map *data)    { return ((Value_Map *) Class2_Array::Get_LE (data)); }

	Value_Map * First (void)                { return ((Value_Map *) Class2_Array::First ()); }
	Value_Map * Next (void)                 { return ((Value_Map *) Class2_Array::Next ()); }
	
	Value_Map * Last (void)                 { return ((Value_Map *) Class2_Array::Last ()); }
	Value_Map * Previous (void)             { return ((Value_Map *) Class2_Array::Previous ()); }

	Value_Map * First_Key (void)            { return ((Value_Map *) Class2_Array::First_Key ()); }
	Value_Map * Next_Key (void)             { return ((Value_Map *) Class2_Array::Next_Key ()); }

	Value_Map * Last_Key (void)             { return ((Value_Map *) Class2_Array::Last_Key ()); }
	Value_Map * Previous_Key (void)         { return ((Value_Map *) Class2_Array::Previous_Key ()); }

	Value_Map * operator[] (int index)      { return (Record (index)); }
};

#endif
