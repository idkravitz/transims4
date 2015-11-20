//*********************************************************
//	Emission_Rate.hpp - emission rate data classes
//*********************************************************

#ifndef EMISSION_RATE_HPP
#define EMISSION_RATE_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Emission_Rate class definition
//---------------------------------------------------------

class Emission_Rate : public Class_Index
{
public:
	Emission_Rate (int id = 0);

	double Rate (void)                { return (rate); }
	double Weight (void)              { return (weight); } 

	void   Rate (double value)        { rate = value; }
	void   Weight (double value)      { weight = value; }

	void   Add_Rate (double value)    { rate += value; }
	void   Add_Weight (double value)  { weight += value; }

private:
	double rate;
	double weight;

};

//---------------------------------------------------------
//	Emission_Rate_Array class definition
//---------------------------------------------------------

class Emission_Rate_Array : public Class_Array
{
public:
	Emission_Rate_Array (int max_records = 0);

	bool Add (Emission_Rate *data = NULL)   { return (Class_Array::Add (data)); }

	Emission_Rate * Record (int index)      { return ((Emission_Rate *) Class_Array::Record (index)); }
	Emission_Rate * Record (void)           { return ((Emission_Rate *) Class_Array::Record ()); }

	Emission_Rate * Get (int id)            { return ((Emission_Rate *) Class_Array::Get (id)); }

	Emission_Rate * First (void)            { return ((Emission_Rate *) Class_Array::First ()); }
	Emission_Rate * Next (void)             { return ((Emission_Rate *) Class_Array::Next ()); }
	
	Emission_Rate * First_Key (void)        { return ((Emission_Rate *) Class_Array::First_Key ()); }
	Emission_Rate * Next_Key (void)         { return ((Emission_Rate *) Class_Array::Next_Key ()); }

	Emission_Rate * operator[] (int index)  { return (Record (index)); }
};

#endif
