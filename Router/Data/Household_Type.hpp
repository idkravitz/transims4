//*********************************************************
//	Household_Type.hpp - household type array
//*********************************************************

#ifndef HOUSEHOLD_TYPE_HPP
#define HOUSEHOLD_TYPE_HPP

#include "HHRec_Array.hpp"

//---------------------------------------------------------
//	Household_Type class definition
//---------------------------------------------------------

class Household_Type : public HHRec_Index
{
public:
	Household_Type (void); 

	int  Type (void)               { return (type); }
	void Type (int value)          { type = value; }

private:
	int type;
};

//---------------------------------------------------------
//	Household_Type_Array class definition
//---------------------------------------------------------

class Household_Type_Array : public HHRec_Array
{
public:
	Household_Type_Array (int max_records = 0);

	Household_Type * New_Record (bool clear = false, int number = 1)
 	                                                  { return ((Household_Type *) HHRec_Array::New_Record (clear, number)); }

	Household_Type * Record (int index)               { return ((Household_Type *) HHRec_Array::Record (index)); }
	Household_Type * Record (void)                    { return ((Household_Type *) HHRec_Array::Record ()); }

	Household_Type * Get (int h, int p=0, int r=0)    { return ((Household_Type *) HHRec_Array::Get (h, p, r));  }
	Household_Type * Get (Household_Type *data)       { return ((Household_Type *) HHRec_Array::Get (data)); }

	Household_Type * Get_GE (int h, int p=0, int r=0) { return ((Household_Type *) HHRec_Array::Get_GE (h, p, r)); }
	Household_Type * Get_GE (Household_Type *data)    { return ((Household_Type *) HHRec_Array::Get_GE (data)); }

	Household_Type * Get_LE (int h, int p=0, int r=0) { return ((Household_Type *) HHRec_Array::Get_LE (h, p, r)); }
	Household_Type * Get_LE (Household_Type *data)    { return ((Household_Type *) HHRec_Array::Get_LE (data)); }

	Household_Type * First (void)                     { return ((Household_Type *) HHRec_Array::First ()); }
	Household_Type * Next (void)                      { return ((Household_Type *) HHRec_Array::Next ()); }
	
	Household_Type * Last (void)                      { return ((Household_Type *) HHRec_Array::Last ()); }
	Household_Type * Previous (void)                  { return ((Household_Type *) HHRec_Array::Previous ()); }

	Household_Type * First_Key (void)                 { return ((Household_Type *) HHRec_Array::First_Key ()); }
	Household_Type * Next_Key (void)                  { return ((Household_Type *) HHRec_Array::Next_Key ()); }

	Household_Type * Last_Key (void)                  { return ((Household_Type *) HHRec_Array::Last_Key ()); }
	Household_Type * Previous_Key (void)              { return ((Household_Type *) HHRec_Array::Previous_Key ()); }

	Household_Type * operator[] (int index)           { return (Record (index)); }
};

#endif
