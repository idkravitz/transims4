//*********************************************************
//	Household_Index.hpp - household index by model cell
//*********************************************************

#ifndef HOUSEHOLD_INDEX_HPP
#define HOUSEHOLD_INDEX_HPP

#include "Complex_Array.hpp"

//---------------------------------------------------------
//	Household_Index structure
//---------------------------------------------------------

typedef struct {
	int   cell;
	int   index;
} Household_Index;

//---------------------------------------------------------
//	Household_Index_Array class definition
//---------------------------------------------------------

class Household_Index_Array : public Complex_Array
{
public:
	Household_Index_Array (int max_records = 0);

	bool Add (Household_Index *data = NULL)         { return (Complex_Array::Add (data)); }
	
	Household_Index * New_Record (bool clear = false, int number = 1)
	                                                { return ((Household_Index *) Complex_Array::New_Record (clear, number)); }

	Household_Index * Get (int cell, int index)     { Key (cell, index); return (Get (&key)); }
	Household_Index * Get (Household_Index *key)    { return ((Household_Index *) Complex_Array::Get (key)); }

	Household_Index * Get_GE (int cell, int index)  { Key (cell, index); return (Get_GE (&key)); }
	Household_Index * Get_GE (Household_Index *key) { return ((Household_Index *) Complex_Array::Get_GE (key)); }

	Household_Index * Record (int index)            { return ((Household_Index *) Complex_Array::Record (index)); }
	Household_Index * Record (void)                 { return ((Household_Index *) Complex_Array::Record ()); }

	Household_Index * First (void)                  { return ((Household_Index *) Complex_Array::First ()); }
	Household_Index * Next (void)                   { return ((Household_Index *) Complex_Array::Next ()); }

	Household_Index * First_Key (void)              { return ((Household_Index *) Complex_Array::First_Key ()); }
	Household_Index * Next_Key (void)               { return ((Household_Index *) Complex_Array::Next_Key ()); }

	Household_Index * operator[] (int index)        { return (Record (index)); }

private:
	void Key (int cell, int index)                  { key.cell = cell; key.index = index; }

	static Household_Index key;
};
#endif
