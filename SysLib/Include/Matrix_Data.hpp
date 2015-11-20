//*********************************************************
//	Matrix_Data.hpp - matrix data classes
//*********************************************************

#ifndef MATRIX_DATA_HPP
#define MATRIX_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Matrix_Key class definition
//---------------------------------------------------------

class Matrix_Key
{
public:
	Matrix_Key (int num_periods = 1, int num_zones = 0);

	bool Set_Key (int num_periods = 1, int num_zones = 0);

	int Key (int org, int des, int period = 1);
	void Key (int key, int *org, int *des, int *period);
	void Key (int key, int *org, int *des);

private:
	int zone_bits, zone_mask, period_mask;
};

//---------------------------------------------------------
//	Matrix_Data class definition
//---------------------------------------------------------

class Matrix_Data : public Class_Index
{
public:
	Matrix_Data (int key = 0);
	virtual ~Matrix_Data (void)  {}

	int  Data (void)                  { return (data); }

	void Data (int value)             { data = value; }

	void Add_Data (int value = 1)     { data += value; }

private:
	int data;
};

//---------------------------------------------------------
//	Matrix_Array class definition
//---------------------------------------------------------

class Matrix_Array : public Class_Array, public Matrix_Key
{
public:
	Matrix_Array (int num_periods = 1, int num_zones = 0, int max_records = 0);

	bool Add (Matrix_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Matrix_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Matrix_Data *) Class_Array::New_Record (clear, number)); }

	Matrix_Data * Record (int index)     { return ((Matrix_Data *) Class_Array::Record (index)); }
	Matrix_Data * Record (void)          { return ((Matrix_Data *) Class_Array::Record ()); }

	Matrix_Data * Get (int key)          { return ((Matrix_Data *) Class_Array::Get (key)); }
	Matrix_Data * Get (Matrix_Data *key) { return ((Matrix_Data *) Class_Array::Get (key)); }

	Matrix_Data * Get (int org, int des, int period = 1) { return (Get (Key (org, des, period))); }

	Matrix_Data * First (void)           { return ((Matrix_Data *) Class_Array::First ()); }
	Matrix_Data * Next (void)            { return ((Matrix_Data *) Class_Array::Next ()); }
	
	Matrix_Data * Last (void)            { return ((Matrix_Data *) Class_Array::Last ()); }
	Matrix_Data * Previous (void)        { return ((Matrix_Data *) Class_Array::Previous ()); }

	Matrix_Data * First_Key (void)       { return ((Matrix_Data *) Class_Array::First_Key ()); }
	Matrix_Data * Next_Key (void)        { return ((Matrix_Data *) Class_Array::Next_Key ()); }

	Matrix_Data * operator[] (int index) { return (Record (index)); }
};

#endif
