//******************************************************** 
//	Tour_Data.hpp - Tour Data
//********************************************************

#ifndef TOUR_DATA_HPP
#define TOUR_DATA_HPP

#include "Complex_Array.hpp"

//---------------------------------------------------------
//	Tour_Data structure
//---------------------------------------------------------

typedef struct
{
	int  household;
	int  person;
	int  start;
	int  origin;
	int  arrive;
	int  destination;
	int  vehicle;
	int  purpose;
	int  mode;
	int  constraint;
} Tour_Data;

//---------------------------------------------------------
//	Tour_Data_Array class definition
//---------------------------------------------------------

class Tour_Data_Array : public Complex_Array
{
public:
	Tour_Data_Array (void) : Complex_Array (sizeof (Tour_Data), 3, false, 20, false) {}

	bool Add (Tour_Data *data = NULL)       { return (Complex_Array::Add (data)); }

	int Get_Index (Tour_Data *data)         { return (Complex_Array::Get_Index (data)); }	

	Tour_Data * Record (int index)          { return ((Tour_Data *) Complex_Array::Record (index)); }
	Tour_Data * Record (void)               { return ((Tour_Data *) Complex_Array::Record ()); }

	Tour_Data * Get (Tour_Data *data)       { return ((Tour_Data *) Complex_Array::Get (data)); }

	Tour_Data * First (void)	            { return ((Tour_Data *) Complex_Array::First ()); }
	Tour_Data * Next (void)                 { return ((Tour_Data *) Complex_Array::Next ()); }

	Tour_Data * First_Key (void)            { return ((Tour_Data *) Complex_Array::First_Key ()); }
	Tour_Data * Next_Key (void)             { return ((Tour_Data *) Complex_Array::Next_Key ()); }
};

#endif
