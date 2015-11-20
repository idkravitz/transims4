//*********************************************************
//	Traveler_Data.hpp - traveler data classes
//*********************************************************

#ifndef TRAVELER_DATA_HPP
#define TRAVELER_DATA_HPP

#include "Static_Service.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Traveler_Data class definition
//---------------------------------------------------------

class Traveler_Data : public Class2_Index, public Static_Scale
{
public:
	Traveler_Data (int traveler = 0, int time = 0);
	virtual ~Traveler_Data (void)   {}

	int  Traveler (void)          { return (Key1 ()); }
	int  Household (void)         { return (Key1 () / Traveler_Scale ()); }
	int  Person (void)            { return (Key1 () % Traveler_Scale ()); }
	int  Time (void)              { return (Key2 ()); }
	int  Vehicle (void)           { return (vehicle); }
	int  Link_Dir (void)          { return (link_dir); }
	int  Link (void)              { return ((link_dir >> 1)); }
	int  Dir (void)               { return ((link_dir & 1)); }
	int  Offset (void)            { return (offset); }
	int  Lane (void)              { return (lane); }
	int  Distance (void)          { return (distance); }
	int  Speed (void)             { return (speed); }

	void Traveler (int value)     { Key1 (value); }
	void Household (int value)    { Key1 (value * Traveler_Scale () + Person ()); }
	void Person (int value)       { Key1 (Household () * Traveler_Scale () + value); }
	void Time (int value)         { Key2 (value); }
	void Vehicle (int value)      { vehicle = value; }
	void Link_Dir (int value)     { link_dir = value; }
	void Link (int value)         { link_dir = (value << 1) + Dir (); }
	void Dir (int value)          { link_dir = (Link () << 1) + value; }
	void Offset (int value)       { offset = (unsigned short) value; }
	void Lane (int value)         { lane = (short) value; }
	void Distance (int value)     { distance = (short) value; }
	void Speed (int value)        { speed = (short) value; }

private:
	int   vehicle;
	int   link_dir;
	unsigned short offset;
	short distance;
	short lane;
	short speed;
};

//---------------------------------------------------------
//	Traveler_Array class definition
//---------------------------------------------------------

class Traveler_Array : public Class2_Array
{
public:
	Traveler_Array (int max_records = 0);

	bool Add (Traveler_Data *data = NULL)        { return (Class2_Array::Add (data)); }
	
	Traveler_Data * New_Record (bool clear = false, int number = 1)
 	                                             { return ((Traveler_Data *) Class2_Array::New_Record (clear, number)); }

	Traveler_Data * Record (int index)           { return ((Traveler_Data *) Class2_Array::Record (index)); }
	Traveler_Data * Record (void)                { return ((Traveler_Data *) Class2_Array::Record ()); }

	Traveler_Data * Get (int key1, int key2)     { return ((Traveler_Data *) Class2_Array::Get (key1, key2)); }
	Traveler_Data * Get (Traveler_Data *data)    { return ((Traveler_Data *) Class2_Array::Get (data)); }

	Traveler_Data * Get_GE (int key1, int key2)  { return ((Traveler_Data *) Class2_Array::Get_GE (key1, key2)); }
	Traveler_Data * Get_GE (Traveler_Data *data) { return ((Traveler_Data *) Class2_Array::Get_GE (data)); }

	Traveler_Data * Get_LE (int key1, int key2)  { return ((Traveler_Data *) Class2_Array::Get_LE (key1, key2)); }
	Traveler_Data * Get_LE (Traveler_Data *data) { return ((Traveler_Data *) Class2_Array::Get_LE (data)); }

	Traveler_Data * First (void)                 { return ((Traveler_Data *) Class2_Array::First ()); }
	Traveler_Data * Next (void)                  { return ((Traveler_Data *) Class2_Array::Next ()); }
	
	Traveler_Data * Last (void)                  { return ((Traveler_Data *) Class2_Array::Last ()); }
	Traveler_Data * Previous (void)              { return ((Traveler_Data *) Class2_Array::Previous ()); }

	Traveler_Data * First_Key (void)             { return ((Traveler_Data *) Class2_Array::First_Key ()); }
	Traveler_Data * Next_Key (void)              { return ((Traveler_Data *) Class2_Array::Next_Key ()); }

	Traveler_Data * Last_Key (void)              { return ((Traveler_Data *) Class2_Array::Last_Key ()); }
	Traveler_Data * Previous_Key (void)          { return ((Traveler_Data *) Class2_Array::Previous_Key ()); }

	Traveler_Data * operator[] (int index)       { return (Record (index)); }
};

#endif

