//*********************************************************
//	Park_Ride_Data.hpp - park-&-ride data data classes
//*********************************************************

#ifndef PARK_RIDE_DATA_HPP
#define PARK_RIDE_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Park_Ride_Data class definition
//---------------------------------------------------------

class Park_Ride_Data
{
public:
	Park_Ride_Data (void);

	int  Parking (void)          { return (parking); }
	int  X (void)                { return (x); }
	int  Y (void)                { return (y); }
	
	void Parking (int value)     { parking = value; }
	void X (int value)	         { x = value; }
	void Y (int value)           { y = value; }

private:
	int parking;
	int x;
	int y;
};

//---------------------------------------------------------
//	Park_Ride_Array
//---------------------------------------------------------

class Park_Ride_Array : public Data_Array
{
public:
	Park_Ride_Array (int max_records = 0);

	bool Add (Park_Ride_Data *data)          { return (Data_Array::Add ((void *) data)); }

	Park_Ride_Data * First (void)            { return ((Park_Ride_Data *) Data_Array::First ()); }
	Park_Ride_Data * Next (void)             { return ((Park_Ride_Data *) Data_Array::Next ()); }

	Park_Ride_Data * Last (void)             { return ((Park_Ride_Data *) Data_Array::Last ()); }
	Park_Ride_Data * Previous (void)         { return ((Park_Ride_Data *) Data_Array::Previous ()); }

	Park_Ride_Data * operator[] (int index)  { return ((Park_Ride_Data *) Record (index)); }
};

#endif
