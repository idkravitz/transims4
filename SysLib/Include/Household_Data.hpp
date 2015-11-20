//*********************************************************
//	Household_Data.hpp - household data classes
//*********************************************************

#ifndef HOUSEHOLD_DATA_HPP
#define HOUSEHOLD_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Household_Data class definition
//---------------------------------------------------------

class Household_Data : public Class_Index
{
public:
	Household_Data (int id = 0);
	virtual ~Household_Data (void)  {}

	int  Household (void)           { return (ID ()); }
	int  Location (void)			{ return (location); }
	int  Persons (void)				{ return (persons); }
	int  Workers (void)				{ return (workers); }
	int  Vehicles (void)			{ return (vehicles); }	
	int  Type (void)                { return (type); }

	void Household (int value)      { ID (value); }
	void Location (int value)		{ location = value; }
	void Persons (int value)		{ persons = value; }
	void Workers (int value)		{ workers = value; }
	void Vehicles (int value)		{ vehicles = value; }
	void Type (int value)	        { type = value; }

private:
	int location, persons, workers, vehicles, type;
};

//---------------------------------------------------------
//	Household_Array class definition
//---------------------------------------------------------

class Household_Array : public Class_Array
{
public:
	Household_Array (int max_records = 0);

	bool Add (Household_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Household_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Household_Data *) Class_Array::New_Record (clear, number)); }

	Household_Data * Record (int index)     { return ((Household_Data *) Class_Array::Record (index)); }
	Household_Data * Record (void)          { return ((Household_Data *) Class_Array::Record ()); }

	Household_Data * Get (int id)           { return ((Household_Data *) Class_Array::Get (id)); }
	Household_Data * Get_GE (int id)        { return ((Household_Data *) Class_Array::Get_GE (id)); }
	Household_Data * Get_LE (int id)        { return ((Household_Data *) Class_Array::Get_LE (id)); }

	Household_Data * First (void)           { return ((Household_Data *) Class_Array::First ()); }
	Household_Data * Next (void)            { return ((Household_Data *) Class_Array::Next ()); }
	
	Household_Data * Last (void)            { return ((Household_Data *) Class_Array::Last ()); }
	Household_Data * Previous (void)        { return ((Household_Data *) Class_Array::Previous ()); }

	Household_Data * First_Key (void)       { return ((Household_Data *) Class_Array::First_Key ()); }
	Household_Data * Next_Key (void)        { return ((Household_Data *) Class_Array::Next_Key ()); }

	Household_Data * operator[] (int index) { return (Record (index)); }
};

#endif
