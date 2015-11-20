//*********************************************************
//	Emission_Rate.cpp - emission rate data classes
//*********************************************************

#include "Emission_Rate.hpp"

//---------------------------------------------------------
//	Emission_Rate constructor
//---------------------------------------------------------

Emission_Rate::Emission_Rate (int id) : Class_Index (id)
{
	Rate (0.0);
	Weight (0.0);
}

//---------------------------------------------------------
//	Emission_Rate_Array constructor
//---------------------------------------------------------

Emission_Rate_Array::Emission_Rate_Array (int max_records) : 
	Class_Array (sizeof (Emission_Rate), max_records)
{
}
