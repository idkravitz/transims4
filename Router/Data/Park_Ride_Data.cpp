//*********************************************************
//	Park_Ride_Data.cpp - park-&-ride data classes
//*********************************************************

#include "Park_Ride_Data.hpp"

//---------------------------------------------------------
//	Park_Ride_Data constructor
//---------------------------------------------------------

Park_Ride_Data::Park_Ride_Data (void)
{
	Parking (0);
	X (0);
	Y (0);
}

//---------------------------------------------------------
//	Park_Ride_Array constructor
//---------------------------------------------------------

Park_Ride_Array::Park_Ride_Array (int max_records) : 
	Data_Array (sizeof (Park_Ride_Data), max_records)
{
}
