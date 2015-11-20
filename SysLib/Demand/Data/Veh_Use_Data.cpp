//*********************************************************
//	Veh_Use_Data.cpp - vehicle use data classes
//*********************************************************

#include "Veh_Use_Data.hpp"

//---------------------------------------------------------
//	Veh_Use_Data constructor
//---------------------------------------------------------

Veh_Use_Data::Veh_Use_Data (void)
{
	Vehicle (0);
	Start (0);
	End (0);
	Driver (0);
}

//---------------------------------------------------------
//	Veh_Use_Array constructor
//---------------------------------------------------------

Veh_Use_Array::Veh_Use_Array (int max_records) : 
	Data_Array (sizeof (Veh_Use_Data), max_records)
{
}
