//*********************************************************
//	TLen_Skim_Data.hpp - time length skim data classes
//*********************************************************

#ifndef TLEN_SKIM_DATA_HPP
#define TLEN_SKIM_DATA_HPP

#include "Class_Array.hpp"
#include "Matrix_Data.hpp"

//---------------------------------------------------------
//	Time_Len_Skim_Data typedef
//---------------------------------------------------------

typedef struct {
	int id;
	int count;
	int time;
	int length;
} Time_Len_Skim_Data;

//---------------------------------------------------------
//	Time_Len_Skim_Array class definition
//---------------------------------------------------------

class Time_Len_Skim_Array : public Index_Array, public Matrix_Key
{
public:
	Time_Len_Skim_Array (int num_periods = 1, int num_zones = 0, int max_records = 0);

	bool Add (Time_Len_Skim_Data *data = NULL)  { return (Index_Array::Add (data)); }
	
	Time_Len_Skim_Data * New_Record (bool clear = false, int number = 1)
	                                            { return ((Time_Len_Skim_Data *) Index_Array::New_Record (clear, number)); }

	Time_Len_Skim_Data * Record (int index)     { return ((Time_Len_Skim_Data *) Index_Array::Record (index)); }
	Time_Len_Skim_Data * Record (void)          { return ((Time_Len_Skim_Data *) Index_Array::Record ()); }

	Time_Len_Skim_Data * Get (int *key)         { return ((Time_Len_Skim_Data *) Index_Array::Get (key)); }
	Time_Len_Skim_Data * Get (Time_Len_Skim_Data *key)   { return ((Time_Len_Skim_Data *) Index_Array::Get (key)); }
	Time_Len_Skim_Data * Get (int org, int des, int period = 1) { int key = Key (org, des, period); return (Get (&key)); }

	Time_Len_Skim_Data * First (void)           { return ((Time_Len_Skim_Data *) Index_Array::First ()); }
	Time_Len_Skim_Data * Next (void)            { return ((Time_Len_Skim_Data *) Index_Array::Next ()); }
	
	Time_Len_Skim_Data * Last (void)            { return ((Time_Len_Skim_Data *) Index_Array::Last ()); }
	Time_Len_Skim_Data * Previous (void)        { return ((Time_Len_Skim_Data *) Index_Array::Previous ()); }

	Time_Len_Skim_Data * First_Key (void)       { return ((Time_Len_Skim_Data *) Index_Array::First_Key ()); }
	Time_Len_Skim_Data * Next_Key (void)        { return ((Time_Len_Skim_Data *) Index_Array::Next_Key ()); }

	Time_Len_Skim_Data * operator[] (int index) { return (Record (index)); }
};

#endif
