//*********************************************************
//	Veh_Use_Data.hpp - vehicle use data classes
//*********************************************************

#ifndef VEH_USE_DATA_HPP
#define VEH_USE_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Veh_Use_Data class definition
//---------------------------------------------------------

class Veh_Use_Data
{
public:
	Veh_Use_Data (void);

	int  Vehicle (void)            { return (vehicle); }
	int  Start (void)              { return (start); }
	int  End (void)                { return (end); }
	int  Driver (void)             { return (driver); }
	
	void Vehicle (int value)       { vehicle = value; }
	void Start (int value)         { start = value; }
	void End (int value)           { end = value; }
	void Driver (int value)        { driver = value; }

private:
	int vehicle;
	int start;
	int end;
	int driver;
};

//---------------------------------------------------------
//	Veh_Use_Array
//---------------------------------------------------------

class Veh_Use_Array : public Data_Array
{
public:
	Veh_Use_Array (int max_records = 0);

	bool Add (Veh_Use_Data *data)          { return (Data_Array::Add ((void *) data)); }

	Veh_Use_Data * First (void)            { return ((Veh_Use_Data *) Data_Array::First ()); }
	Veh_Use_Data * Next (void)             { return ((Veh_Use_Data *) Data_Array::Next ()); }

	Veh_Use_Data * Last (void)             { return ((Veh_Use_Data *) Data_Array::Last ()); }
	Veh_Use_Data * Previous (void)         { return ((Veh_Use_Data *) Data_Array::Previous ()); }

	Veh_Use_Data * operator[] (int index)  { return ((Veh_Use_Data *) Record (index)); }
};

#endif
