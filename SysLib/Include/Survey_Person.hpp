//*********************************************************
//	Survey_Person.hpp - extended population class
//*********************************************************

#ifndef SURVEY_PERSON_HPP
#define SURVEY_PERSON_HPP

#include "Population_Data.hpp"

//---------------------------------------------------------
//	Survey_Person class definition
//---------------------------------------------------------

class Survey_Person : public Population_Data
{
public:
	Survey_Person (void);

	int  Count (void)              { return (count); }
	void Count (int value)         { count = value; }

	void Add_Count (int value = 1) { count += value; }

private:
	int count;
};

//---------------------------------------------------------
//	Survey_Person_Array class definition
//---------------------------------------------------------

class Survey_Person_Array : public Population_Array
{
public:
	Survey_Person_Array (int max_records = 0);
	virtual ~Survey_Person_Array (void)     { Clear (); }

	bool Add (Survey_Person *data = NULL)   { return (Population_Array::Add (data)); }
	
	Survey_Person * New_Record (bool clear = false, int number = 1)
	                                        { return ((Survey_Person *) Population_Array::New_Record (clear, number)); }

	Survey_Person * Get (int hh, int per)   { return ((Survey_Person *) Population_Array::Get (hh, per)); }
	Survey_Person * Get_GE (int hhold)      { return ((Survey_Person *) Population_Array::Get_GE (hhold)); }

	Survey_Person * Record (int index)      { return ((Survey_Person *) Population_Array::Record (index)); }
	Survey_Person * Record (void)           { return ((Survey_Person *) Population_Array::Record ()); }

	Survey_Person * First (void)            { return ((Survey_Person *) Population_Array::First ()); }
	Survey_Person * Next (void)             { return ((Survey_Person *) Population_Array::Next ()); }
	
	Survey_Person * Last (void)             { return ((Survey_Person *) Population_Array::Last ()); }
	Survey_Person * Previous (void)         { return ((Survey_Person *) Population_Array::Previous ()); }
	
	Survey_Person * First_Key (void)        { return ((Survey_Person *) Population_Array::First_Key ()); }
	Survey_Person * Next_Key (void)         { return ((Survey_Person *) Population_Array::Next_Key ()); }

	Survey_Person * operator[] (int index)  { return (Record (index)); }
};

#endif
