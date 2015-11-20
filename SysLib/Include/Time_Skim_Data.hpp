//*********************************************************
//	Time_Skim_Data.hpp - time skim data classes
//*********************************************************

#ifndef Time_Skim_Data_HPP
#define Time_Skim_Data_HPP

#include "Class_Array.hpp"
#include "Matrix_Data.hpp"

//---------------------------------------------------------
//	Time_Skim_Data typedef
//---------------------------------------------------------

typedef struct {
	int id;
	int count;
	int time;
} Time_Skim_Data;

//---------------------------------------------------------
//	Time_Skim_Array class definition
//---------------------------------------------------------

class Time_Skim_Array : public Index_Array, public Matrix_Key
{
public:
	Time_Skim_Array (int num_periods = 1, int num_zones = 0, int max_records = 0);

	bool Add (Time_Skim_Data *data = NULL)  { return (Index_Array::Add (data)); }
	
	Time_Skim_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Time_Skim_Data *) Index_Array::New_Record (clear, number)); }

	Time_Skim_Data * Record (int index)     { return ((Time_Skim_Data *) Index_Array::Record (index)); }
	Time_Skim_Data * Record (void)          { return ((Time_Skim_Data *) Index_Array::Record ()); }

	Time_Skim_Data * Get (int *key)         { return ((Time_Skim_Data *) Index_Array::Get (key)); }
	Time_Skim_Data * Get (Time_Skim_Data *key)   { return ((Time_Skim_Data *) Index_Array::Get (key)); }
	Time_Skim_Data * Get (int org, int des, int period = 1) { int key = Key (org, des, period); return (Get (&key)); }

	Time_Skim_Data * First (void)           { return ((Time_Skim_Data *) Index_Array::First ()); }
	Time_Skim_Data * Next (void)            { return ((Time_Skim_Data *) Index_Array::Next ()); }
	
	Time_Skim_Data * Last (void)            { return ((Time_Skim_Data *) Index_Array::Last ()); }
	Time_Skim_Data * Previous (void)        { return ((Time_Skim_Data *) Index_Array::Previous ()); }

	Time_Skim_Data * First_Key (void)       { return ((Time_Skim_Data *) Index_Array::First_Key ()); }
	Time_Skim_Data * Next_Key (void)        { return ((Time_Skim_Data *) Index_Array::Next_Key ()); }

	Time_Skim_Data * operator[] (int index) { return (Record (index)); }
};

#endif
