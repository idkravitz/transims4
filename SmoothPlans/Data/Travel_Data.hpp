//*********************************************************
//	Travel_Data.hpp - plan travel data classes
//*********************************************************

#ifndef TRAVEL_DATA_HPP
#define TRAVEL_DATA_HPP

#include "Class_Array.hpp"
#include "Link_Time.hpp"

//---------------------------------------------------------
//	Travel_Data class definition
//---------------------------------------------------------

class Travel_Data : public Class_Index
{
public:
	Travel_Data (int traveler = 0);

	int  Traveler (void)                { return (ID ()); }
	int  Change (void)                  { return (change); }
	int  Min_Change (void)              { return (min_change); }
	int  Max_Change (void)              { return (max_change); }
	bool Flag (void)                    { return (flag); }

	void Traveler (int value)           { ID (value); }
	void Change (int value)             { change = value; }
	void Min_Change (int value)         { min_change = value; };
	void Max_Change (int value)         { max_change = value; }
	void Flag (bool value)              { flag = value; }

	bool Add_Dir (Link_Time *rec)       { return (dir.Add (rec)); }

	void Clear (void)                   { dir.Clear (); }

	Link_Time * First (void)            { return (dir.First ()); }
	Link_Time * Next (void)             { return (dir.Next ()); }

	int  Optimize (void)                { return (dir.Max_Records (dir.Num_Records ())); }

	int  Next_Index (void)	            { return (next); }
	void Next_Index (int index)         { next = index; }
	
private:
	int  change;
	int  min_change;
	int  max_change;
	int  next;
	bool flag;
	Link_Time_Array dir;
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
