//*********************************************************
//	Type_Data.hpp - survey type array class
//*********************************************************

#ifndef TYPE_DATA_HPP
#define TYPE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Type_Data class definition
//---------------------------------------------------------

class Type_Data : public Class_Index
{
public:
	Type_Data (int type = 0);

	int    Type (void)                 { return (ID ()); }
	int    Household (void)            { return (hhold); }
	int    Num_Survey (void)           { return (nsurvey); }
	int    Num_HHold (void)            { return (nhhold); }
	double Cum_Weight (void)           { return (weight); }

	void   Type (int value)            { ID (value); }
	void   Household (int value)       { hhold = value; }
	void   Num_Survey (int value)      { nsurvey = value; }
	void   Num_HHold (int value)       { nhhold = value; }
	void   Cum_Weight (double value)   { weight = value; }

	int    Add_Survey (void)           { return (++nsurvey); }
	int    Add_HHold (void)            { return (++nhhold); }
	double Add_Weight (double value)   { return (weight += value); }

private:
	int    hhold;
	int    nsurvey;
	int    nhhold;
	double weight;
};

//---------------------------------------------------------
//	Type_Array class definition
//---------------------------------------------------------

class Type_Array : public Class_Array
{
public:
	Type_Array (int max_records = 0);

	bool Add (Type_Data *data = NULL)     { return (Class_Array::Add (data)); }
	
	Type_Data * New_Record (bool clear = false, int number = 1)
	                                      { return ((Type_Data *) Class_Array::New_Record (clear, number)); }

	Type_Data * Get (int type)            { return ((Type_Data *) Class_Array::Get (type)); }

	Type_Data * Record (int index)        { return ((Type_Data *) Class_Array::Record (index)); }
	Type_Data * Record (void)             { return ((Type_Data *) Class_Array::Record ()); }

	Type_Data * First (void)              { return ((Type_Data *) Class_Array::First ()); }
	Type_Data * Next (void)               { return ((Type_Data *) Class_Array::Next ()); }
	
	Type_Data * First_Key (void)          { return ((Type_Data *) Class_Array::First_Key ()); }
	Type_Data * Next_Key (void)           { return ((Type_Data *) Class_Array::Next_Key ()); }

	Type_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
