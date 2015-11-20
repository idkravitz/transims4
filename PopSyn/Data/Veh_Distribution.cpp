//*********************************************************
//	Veh_Distribution.cpp - vehicle type distribution class
//*********************************************************

#include "Veh_Distribution.hpp"

//---------------------------------------------------------
//	Veh_Distribution constructor
//---------------------------------------------------------

Veh_Distribution::Veh_Distribution (void)
{
	Type (0);
	Sub_Type (0);
	Share (0.0);
}

//---------------------------------------------------------
//	Veh_Distribution_Array constructor
//---------------------------------------------------------

Veh_Distribution_Array::Veh_Distribution_Array (int max_records) : 
	Data_Array (sizeof (Veh_Distribution), max_records)
{
}
