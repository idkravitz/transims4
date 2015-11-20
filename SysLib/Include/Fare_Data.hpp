//*********************************************************
//	Fare_Data.hpp - network transit fare data
//*********************************************************

#ifndef FARE_DATA_HPP
#define FARE_DATA_HPP

#include "Complex_Array.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Fare_Data class definition
//---------------------------------------------------------

typedef struct {
	short to_zone;
	short from_zone;
	char type;
	char period;
	char to_mode;
	char from_mode;
	int  fare;
} Fare_Data;

//---------------------------------------------------------
//	Fare_Array class definition
//---------------------------------------------------------

class Fare_Array : public Complex_Array
{
public:
	Fare_Array (int max_records = 0);

	bool Add (Fare_Data *data = NULL)    { return (Complex_Array::Add (data)); }
	
	Fare_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Fare_Data *) Complex_Array::New_Record (clear, number)); }

	Fare_Data * Record (int index)       { return ((Fare_Data *) Complex_Array::Record (index)); }
	Fare_Data * Record (void)            { return ((Fare_Data *) Complex_Array::Record ()); }

	Fare_Data * Get (Fare_Data *data)    { return ((Fare_Data *) Complex_Array::Get (data)); }
 
	Fare_Data * First (void)             { return ((Fare_Data *) Complex_Array::First ()); }
	Fare_Data * Next (void)              { return ((Fare_Data *) Complex_Array::Next ()); }
	
	Fare_Data * Last (void)              { return ((Fare_Data *) Complex_Array::Last ()); }
	Fare_Data * Previous (void)          { return ((Fare_Data *) Complex_Array::Previous ()); }

	Fare_Data * First_Key (void)         { return ((Fare_Data *) Complex_Array::First_Key ()); }
	Fare_Data * Next_Key (void)          { return ((Fare_Data *) Complex_Array::Next_Key ()); }

	Fare_Data * operator[] (int index)   { return (Record (index)); }

	int  Period (int time)               { return (time_period.Period (time)); }
	bool Periods (char *breaks)          { return (time_period.Add_Breaks (breaks)); }
	int  Num_Periods (void)              { return (time_period.Num_Records ()); }
	int  Time_Step (char *time)          { return (time_period.Step (time)); }
	char *Period_Format (int num)        { return (time_period.Range_Format (num)); }

private:
	Time_Range time_period;
};

#endif
