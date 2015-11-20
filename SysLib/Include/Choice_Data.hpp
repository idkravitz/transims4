//*********************************************************
//	Choice_Data.hpp - extended location data class
//*********************************************************

#ifndef CHOICE_DATA_HPP
#define CHOICE_DATA_HPP

#include "Location_Data.hpp"

//---------------------------------------------------------
//	Choice_Data class definition
//---------------------------------------------------------

class Choice_Data : public Location_Data
{
public:
	Choice_Data (void);

	int    Mode (void)                 { return (mode); }
	int    List (void)                 { return (list); }
	double Utility (void)              { return (utility); }

	void   Mode (int value)            { mode = value; }
	void   List (int value)            { list = value; }
	void   Utility (double value)      { utility = value; }

private:
	int    mode;
	int    list;
	double utility;
};

//---------------------------------------------------------
//	Choice_Array class definition
//---------------------------------------------------------

class Choice_Array : public Location_Array
{
public:
	Choice_Array (int max_records = 0);
	virtual ~Choice_Array (void)            { Clear (); }

	bool Add (Choice_Data *data = NULL)     { return (Location_Array::Add (data)); }
	
	Choice_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((Choice_Data *) Location_Array::New_Record (clear, number)); }

	Choice_Data * Get (int ID)              { return ((Choice_Data *) Location_Array::Get (ID)); }
	Choice_Data * Get_GE (int ID)           { return ((Choice_Data *) Location_Array::Get_GE (ID)); }
	Choice_Data * Get_LE (int ID)           { return ((Choice_Data *) Location_Array::Get_LE (ID)); }

	Choice_Data * Record (int index)        { return ((Choice_Data *) Location_Array::Record (index)); }
	Choice_Data * Record (void)             { return ((Choice_Data *) Location_Array::Record ()); }

	Choice_Data * First (void)              { return ((Choice_Data *) Location_Array::First ()); }
	Choice_Data * Next (void)               { return ((Choice_Data *) Location_Array::Next ()); }
	
	Choice_Data * Last (void)               { return ((Choice_Data *) Location_Array::Last ()); }
	Choice_Data * Previous (void)           { return ((Choice_Data *) Location_Array::Previous ()); }
	
	Choice_Data * First_Key (void)          { return ((Choice_Data *) Location_Array::First_Key ()); }
	Choice_Data * Next_Key (void)           { return ((Choice_Data *) Location_Array::Next_Key ()); }

	Choice_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
