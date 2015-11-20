//*********************************************************
//	Factor_Data.hpp - factor data classes
//*********************************************************

#ifndef FACTOR_DATA_HPP
#define FACTOR_DATA_HPP

#include "Class_Array.hpp"
#include "Matrix_Data.hpp"

//---------------------------------------------------------
//	Factor_Data class definition
//---------------------------------------------------------

class Factor_Data : public Class_Index
{
public:
	Factor_Data (int key = 0);

	double Factor (void)                 { return (factor); }
	double Bucket (void)                 { return (bucket); }

	void   Factor (double value)         { factor = value; }
	void   Bucket (double value)         { bucket = value; }

	int Bucket_Factor (double value);

private:
	double factor;
	double bucket;
};

//---------------------------------------------------------
//	Factor_Array class definition
//---------------------------------------------------------

class Factor_Array : public Class_Array, public Matrix_Key
{
public:
	Factor_Array (int num_periods = 1, int num_zones = 0, int max_records = 0);

	bool Add (Factor_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Factor_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Factor_Data *) Class_Array::New_Record (clear, number)); }

	Factor_Data * Record (int index)     { return ((Factor_Data *) Class_Array::Record (index)); }
	Factor_Data * Record (void)          { return ((Factor_Data *) Class_Array::Record ()); }

	Factor_Data * Get (int key)          { return ((Factor_Data *) Class_Array::Get (key)); }
	Factor_Data * Get (Factor_Data *key) { return ((Factor_Data *) Class_Array::Get (key)); }

	Factor_Data * Get (int org, int des, int period = 1) { return (Get (Key (org, des, period))); }

	Factor_Data * First (void)           { return ((Factor_Data *) Class_Array::First ()); }
	Factor_Data * Next (void)            { return ((Factor_Data *) Class_Array::Next ()); }
	
	Factor_Data * Last (void)            { return ((Factor_Data *) Class_Array::Last ()); }
	Factor_Data * Previous (void)        { return ((Factor_Data *) Class_Array::Previous ()); }

	Factor_Data * First_Key (void)       { return ((Factor_Data *) Class_Array::First_Key ()); }
	Factor_Data * Next_Key (void)        { return ((Factor_Data *) Class_Array::Next_Key ()); }

	Factor_Data * operator[] (int index) { return (Record (index)); }
};

#endif
