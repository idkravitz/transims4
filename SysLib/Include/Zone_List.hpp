//*********************************************************
//	Zone_List.hpp - extended zone data class
//*********************************************************

#ifndef ZONE_LIST_HPP
#define ZONE_LIST_HPP

#include "Zone_Data.hpp"

//---------------------------------------------------------
//	Zone_List class definition
//---------------------------------------------------------

class Zone_List : public Zone_Data
{
public:
	Zone_List (void);

	int  Location (void)             { return (location); }
	int  Min_X (void)                { return (min_x); }
	int  Min_Y (void)                { return (min_y); }
	int  Max_X (void)                { return (max_x); }
	int  Max_Y (void)                { return (max_y); }

	void Location (int value)        { location = value; }
	void Min_X (int value)           { min_x = value; }
	void Min_Y (int value)           { min_y = value; }
	void Max_X (int value)           { max_x = value; }
	void Max_Y (int value)           { max_y = value; }

private:
	int  location;
	int  min_x;
	int  min_y;
	int  max_x;
	int  max_y;
};

//---------------------------------------------------------
//	Zone_List_Array class definition
//---------------------------------------------------------

class Zone_List_Array : public Zone_Array
{
public:
	Zone_List_Array (int max_records = 0);
	virtual ~Zone_List_Array (void)       { Clear (); }

	bool Add (Zone_List *data = NULL)     { return (Zone_Array::Add (data)); }
	
	Zone_List * New_Record (bool clear = false, int number = 1)
	                                      { return ((Zone_List *) Zone_Array::New_Record (clear, number)); }

	Zone_List * Get (int ID)              { return ((Zone_List *) Zone_Array::Get (ID)); }
	Zone_List * Get_GE (int ID)           { return ((Zone_List *) Zone_Array::Get_GE (ID)); }
	Zone_List * Get_LE (int ID)           { return ((Zone_List *) Zone_Array::Get_LE (ID)); }

	Zone_List * Record (int index)        { return ((Zone_List *) Zone_Array::Record (index)); }
	Zone_List * Record (void)             { return ((Zone_List *) Zone_Array::Record ()); }

	Zone_List * First (void)              { return ((Zone_List *) Zone_Array::First ()); }
	Zone_List * Next (void)               { return ((Zone_List *) Zone_Array::Next ()); }
	
	Zone_List * Last (void)               { return ((Zone_List *) Zone_Array::Last ()); }
	Zone_List * Previous (void)           { return ((Zone_List *) Zone_Array::Previous ()); }
	
	Zone_List * First_Key (void)          { return ((Zone_List *) Zone_Array::First_Key ()); }
	Zone_List * Next_Key (void)           { return ((Zone_List *) Zone_Array::Next_Key ()); }

	Zone_List * operator[] (int index)    { return (Record (index)); }
};

#endif
