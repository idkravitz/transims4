//*********************************************************
//	Weight_Data.hpp - survey household weights class
//*********************************************************

#ifndef WEIGHT_DATA_HPP
#define WEIGHT_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Weight_Data class definition
//---------------------------------------------------------

class Weight_Data : public Class_Index
{
public:
	Weight_Data (int household = 0);

	int    Household (void)            { return (ID ()); }
	double Weight (void)               { return (weight); }

	void   Household (int value)       { ID (value); }
	void   Weight (double value)       { weight = value; }

private:
	double weight;
};

//---------------------------------------------------------
//	Weight_Array class definition
//---------------------------------------------------------

class Weight_Array : public Class_Array
{
public:
	Weight_Array (int max_records = 0);

	bool Add (Weight_Data *data = NULL)     { return (Class_Array::Add (data)); }
	
	Weight_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Weight_Data *) Class_Array::New_Record (clear, number)); }

	Weight_Data * Get (int id)              { return ((Weight_Data *) Class_Array::Get (id)); }

	Weight_Data * Record (int index)        { return ((Weight_Data *) Class_Array::Record (index)); }
	Weight_Data * Record (void)             { return ((Weight_Data *) Class_Array::Record ()); }

	Weight_Data * First (void)              { return ((Weight_Data *) Class_Array::First ()); }
	Weight_Data * Next (void)               { return ((Weight_Data *) Class_Array::Next ()); }
	
	Weight_Data * First_Key (void)          { return ((Weight_Data *) Class_Array::First_Key ()); }
	Weight_Data * Next_Key (void)           { return ((Weight_Data *) Class_Array::Next_Key ()); }

	Weight_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
