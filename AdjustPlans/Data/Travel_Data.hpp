	//*********************************************************
//	Travel_Data.hpp - plan travel data classes
//*********************************************************

#ifndef TRAVEL_DATA_HPP
#define TRAVEL_DATA_HPP

#include "Class_Array.hpp"
#include "Integer_Array.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Travel_Data class definition
//---------------------------------------------------------

class Travel_Data : public Class_Index, public Static_Scale 
{
public:
	Travel_Data (int traveler = 0);

	int  Traveler (void)                { return (ID ()); }
	int  Household (void)               { return (ID () / Traveler_Scale ()); }
	int  Person (void)                  { return (ID () % Traveler_Scale ()); }
	int  Weight (void)                  { return (weight); }

	void Traveler (int value)           { ID (value); }
	void Household (int value)          { ID (value * Traveler_Scale () + Person ()); }
	void Person (int value)             { ID (Household () * Traveler_Scale () + value); }
	void Weight (int value)             { weight = value; }
	bool Add_Count (int value)          { return (count.Add (value)); }

	bool Replicate (int max_copy = 100)	{ if (weight < max_copy) { weight++; return (true); } else return (false); }
	bool Delete (void)                  { if (weight > 0) { weight--; return (true); } else return (false); }

	void Clear (void)                   { count.Clear (); }

	int  First (void)                   { return (count.First ()); }
	int  Next (void)                    { return (count.Next ()); }

	int  Optimize (void)                { return (count.Max_Records (count.Num_Records ())); }
	
private:
	int  weight;
	Integer_List count;
};

//---------------------------------------------------------
//	Travel_Array class definition
//---------------------------------------------------------

class Travel_Array : public Class_Array
{
public:
	Travel_Array (int max_records = 0);

	bool Add (Travel_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Travel_Data * New_Record (bool clear = false, int number = 1)
	                                          { return ((Travel_Data *) Class_Array::New_Record (clear, number)); }

	Travel_Data * Record (int index)          { return ((Travel_Data *) Class_Array::Record (index)); }
	Travel_Data * Record (void)               { return ((Travel_Data *) Class_Array::Record ()); }

	Travel_Data * Get (int id)                { return ((Travel_Data *) Class_Array::Get (id)); }

	Travel_Data * First (void)                { return ((Travel_Data *) Class_Array::First ()); }
	Travel_Data * Next (void)                 { return ((Travel_Data *) Class_Array::Next ()); }
	
	Travel_Data * Last (void)                 { return ((Travel_Data *) Class_Array::Last ()); }
	Travel_Data * Previous (void)             { return ((Travel_Data *) Class_Array::Previous ()); }

	Travel_Data * First_Key (void)            { return ((Travel_Data *) Class_Array::First_Key ()); }
	Travel_Data * Next_Key (void)             { return ((Travel_Data *) Class_Array::Next_Key ()); }

	Travel_Data * Last_Key (void)             { return ((Travel_Data *) Class_Array::Last_Key ()); }
	Travel_Data * Previous_Key (void)         { return ((Travel_Data *) Class_Array::Previous_Key ()); }

	Travel_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
