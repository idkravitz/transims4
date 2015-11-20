//*********************************************************
//	Survey_HHold.hpp - extended household class
//*********************************************************

#ifndef SURVEY_HHOLD_HPP
#define SURVEY_HHOLD_HPP

#include "Household_Data.hpp"

//---------------------------------------------------------
//	Survey_HHold class definition
//---------------------------------------------------------

class Survey_HHold : public Household_Data
{
public:
	Survey_HHold (void);
	
	double Weight (void)                { return (weight); }

	void   Weight (double value)        { weight = value; }

private:
	double  weight;
};

//---------------------------------------------------------
//	Survey_HHold_Array class definition
//---------------------------------------------------------

class Survey_HHold_Array : public Household_Array
{
public:
	Survey_HHold_Array (int max_records = 0);

	bool Add (Survey_HHold *data = NULL)     { return (Household_Array::Add (data)); }
	
	Survey_HHold * New_Record (bool clear = false, int number = 1)
	                                         { return ((Survey_HHold *) Household_Array::New_Record (clear, number)); }

	Survey_HHold * Get (int ID)              { return ((Survey_HHold *) Household_Array::Get (ID)); }
	Survey_HHold * Get_GE (int ID)           { return ((Survey_HHold *) Household_Array::Get_GE (ID)); }
	Survey_HHold * Get_LE (int ID)           { return ((Survey_HHold *) Household_Array::Get_LE (ID)); }

	Survey_HHold * Record (int index)        { return ((Survey_HHold *) Household_Array::Record (index)); }
	Survey_HHold * Record (void)             { return ((Survey_HHold *) Household_Array::Record ()); }

	Survey_HHold * First (void)              { return ((Survey_HHold *) Household_Array::First ()); }
	Survey_HHold * Next (void)               { return ((Survey_HHold *) Household_Array::Next ()); }
	
	Survey_HHold * Last (void)               { return ((Survey_HHold *) Household_Array::Last ()); }
	Survey_HHold * Previous (void)           { return ((Survey_HHold *) Household_Array::Previous ()); }
	
	Survey_HHold * First_Key (void)          { return ((Survey_HHold *) Household_Array::First_Key ()); }
	Survey_HHold * Next_Key (void)           { return ((Survey_HHold *) Household_Array::Next_Key ()); }

	Survey_HHold * operator[] (int index)    { return (Record (index)); }
};

#endif
