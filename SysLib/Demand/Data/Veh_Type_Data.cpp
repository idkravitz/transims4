//*********************************************************
//	Veh_Type_Data.cpp - vehicle type data classes
//*********************************************************

#include "Veh_Type_Data.hpp"

//---------------------------------------------------------
//	Veh_Type_Data constructor
//---------------------------------------------------------

Veh_Type_Data::Veh_Type_Data (int type, int subtype) : Class_Index ()
{
	Type (type);
	Sub_Type (subtype);
	Length (0);
	Max_Speed (0);
	Max_Accel (0);
	Max_Decel (0);
	Use (Network_Code::ANY);
	Capacity (0);
	Loading (0);
	Unloading (0);
	Method (false);
	Min_Dwell (0);
	Max_Dwell (0);
}

//---------------------------------------------------------
//	Veh_Type_Array constructor
//---------------------------------------------------------

Veh_Type_Array::Veh_Type_Array (int max_records) : 
	Class_Array (sizeof (Veh_Type_Data), max_records)
{
}
