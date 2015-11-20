//*********************************************************
//	Population_Data.hpp - population data classes
//*********************************************************

#ifndef POPULATION_DATA_HPP
#define POPULATION_DATA_HPP

#include "Static_Service.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Population_Data class definition
//---------------------------------------------------------

class Population_Data : public Class_Index, public Static_Scale 
{
public:
	Population_Data (int hhold = 0, int person = 0);
	virtual ~Population_Data (void)  {}

	int  Traveler (void)           { return (ID ()); }
	int  Household (void)          { return ((ID () / Traveler_Scale ())); }
	int  Person (void)             { return ((ID () % Traveler_Scale ())); }
	int  Relate (void)             { return (relate); }
	int  Age (void)                { return (age); }
	int  Work (void)               { return (work); }
	int  Gender (void)             { return (flags >> 4); }
	int  Drive (void)              { return (flags & 0x0F); }	

	void Traveler (int value)      { ID (value); }
	void Household (int value)     { ID ((value * Traveler_Scale ()) + Person ()); }
	void Person (int value)        { ID ((Household () * Traveler_Scale ()) + value); }
	void Relate (int value)        { relate = (char) value; }
	void Age (int value)           { age = (char) value; }
	void Work (int value)          { work = (char) value; }
	void Gender (int value)        { flags = (char) (Drive () + (value << 4)); }
	void Drive (int value)         { flags = (char) ((flags & 0xF0) + (value & 0x0F)); }

private:
	char relate;
	char age;
	char work;
	char flags;		//---- gender, drive ----
};

//---------------------------------------------------------
//	Population_Array class definition
//---------------------------------------------------------

class Population_Array : public Class_Array
{
public:
	Population_Array (int max_records = 0);

	bool Add (Population_Data *data = NULL)          { return (Class_Array::Add (data)); }
	
	Population_Data * New_Record (bool clear = false, int number = 1)
	                                                 { return ((Population_Data *) Class_Array::New_Record (clear, number)); }

	Population_Data * Record (int index)             { return ((Population_Data *) Class_Array::Record (index)); }
	Population_Data * Record (void)                  { return ((Population_Data *) Class_Array::Record ()); }
	
	Population_Data * Get (Population_Data *data)    { return ((Population_Data *) Class_Array::Get (data)); }
	Population_Data * Get (int hhold, int person)    { Population_Data data (hhold, person); return (Get (&data)); }

	Population_Data * Get_GE (Population_Data *data) { return ((Population_Data *) Class_Array::Get_GE (data)); }
	Population_Data * Get_GE (int hhold)             { Population_Data data (hhold); return (Get_GE (&data)); }

	Population_Data * First (void)                   { return ((Population_Data *) Class_Array::First ()); }
	Population_Data * Next (void)                    { return ((Population_Data *) Class_Array::Next ()); }

	Population_Data * Last (void)                    { return ((Population_Data *) Class_Array::Last ()); }
	Population_Data * Previous (void)                { return ((Population_Data *) Class_Array::Previous ()); }

	Population_Data * First_Key (void)               { return ((Population_Data *) Class_Array::First_Key ()); }
	Population_Data * Next_Key (void)                { return ((Population_Data *) Class_Array::Next_Key ()); }

	Population_Data * operator[] (int index)         { return (Record (index)); }
};

#endif
