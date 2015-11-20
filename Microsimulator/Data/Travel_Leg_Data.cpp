//*********************************************************
//	Travel_Leg_Data.cpp - travel leg data classes
//*********************************************************

#include "Travel_Leg_Data.hpp"

//---------------------------------------------------------
//	Travel_Leg_Data constructor
//---------------------------------------------------------

Travel_Leg_Data::Travel_Leg_Data (void)
{
	Dir (0);
	In_Lane_Low (0);
	In_Lane_High (0);
	In_Best_Low (0);
	In_Best_High (0);
	Out_Lane_Low (0);
	Out_Lane_High (0);
	Out_Best_Low (0);
	Out_Best_High (0);
	Max_Speed (0);
	Stop_Number (0);
	Control (0);
	Connect (0);
}

//---------------------------------------------------------
//	Travel_Leg_Array constructor
//---------------------------------------------------------

Travel_Leg_Array::Travel_Leg_Array (int max_records) : 
	Data_Array (sizeof (Travel_Leg_Data), max_records)
{
}
