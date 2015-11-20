//*********************************************************
//	Movement_Data.hpp - turn movement data
//*********************************************************

#ifndef MOVEMENT_DATA_HPP
#define MOVEMENT_DATA_HPP

#include "Complex_Array.hpp"

//---------------------------------------------------------
//	Movement_Data struct definition
//---------------------------------------------------------

typedef struct {
	int node;
	int in_link;
	int out_link;
	int volume;
} Movement_Data;

//---------------------------------------------------------
//	Movement_Array class definition
//---------------------------------------------------------

class Movement_Array : public Complex_Array
{
public:
	Movement_Array (int max_records = 0);

	bool Add (Movement_Data *data = NULL)      { return (Complex_Array::Add (data)); }
	
	Movement_Data * Record (int index)         { return ((Movement_Data *) Complex_Array::Record (index)); }
	Movement_Data * Record (void)              { return ((Movement_Data *) Complex_Array::Record ()); }

	Movement_Data * Get (Movement_Data *data)  { return ((Movement_Data *) Complex_Array::Get (data)); }

	Movement_Data * First_Key (void)           { return ((Movement_Data *) Complex_Array::First_Key ()); }
	Movement_Data * Next_Key (void)            { return ((Movement_Data *) Complex_Array::Next_Key ()); }

	Movement_Data * operator[] (int index)     { return (Record (index)); }
};
#endif
