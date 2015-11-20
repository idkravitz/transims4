//*********************************************************
//	HHold_Data.hpp - extended household class
//*********************************************************

#ifndef HHOLD_DATA_HPP
#define HHOLD_DATA_HPP

#include "Household_Data.hpp"

//---------------------------------------------------------
//	HHold_Data class definition
//---------------------------------------------------------

class HHold_Data : public Household_Data
{
public:
	HHold_Data (void);

	int  Vehicle_List (void)            { return (vehicle); }
	bool Problem_Flag (void)			{ return (problem); }

	void Vehicle_List (int value)       { vehicle = value; }
	void Problem_Flag (bool flag)		{ problem = flag; }

private:
	int vehicle;
	bool problem;
};

//---------------------------------------------------------
//	HHold_Array class definition
//---------------------------------------------------------

class HHold_Array : public Household_Array
{
public:
	HHold_Array (int max_records = 0);
	virtual ~HHold_Array (void)            { Clear (); }

	bool Add (HHold_Data *data = NULL)     { return (Household_Array::Add (data)); }
	
	HHold_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((HHold_Data *) Household_Array::New_Record (clear, number)); }

	HHold_Data * Get (int ID)              { return ((HHold_Data *) Household_Array::Get (ID)); }
	HHold_Data * Get_GE (int ID)           { return ((HHold_Data *) Household_Array::Get_GE (ID)); }
	HHold_Data * Get_LE (int ID)           { return ((HHold_Data *) Household_Array::Get_LE (ID)); }

	HHold_Data * Record (int index)        { return ((HHold_Data *) Household_Array::Record (index)); }
	HHold_Data * Record (void)             { return ((HHold_Data *) Household_Array::Record ()); }

	HHold_Data * First (void)              { return ((HHold_Data *) Household_Array::First ()); }
	HHold_Data * Next (void)               { return ((HHold_Data *) Household_Array::Next ()); }
	
	HHold_Data * Last (void)               { return ((HHold_Data *) Household_Array::Last ()); }
	HHold_Data * Previous (void)           { return ((HHold_Data *) Household_Array::Previous ()); }
	
	HHold_Data * First_Key (void)          { return ((HHold_Data *) Household_Array::First_Key ()); }
	HHold_Data * Next_Key (void)           { return ((HHold_Data *) Household_Array::Next_Key ()); }

	HHold_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
