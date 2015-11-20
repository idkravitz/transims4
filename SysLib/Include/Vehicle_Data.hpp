//*********************************************************
//	Vehicle_Data.hpp - vehicle data classes
//*********************************************************

#ifndef VEHICLE_DATA_HPP
#define VEHICLE_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Vehicle_Data class definition
//---------------------------------------------------------

class Vehicle_Data : public Class_Index
{
public:
	Vehicle_Data (int vehicle = 0);
	virtual ~Vehicle_Data (void)     {}

	int  Vehicle (void)              { return (ID ()); }
	int  Household (void)            { return (household); }
	int  Location (void)             { return (location); }
	int  Type (void)                 { return (type); }
	int  Sub_Type (void)             { return (sub_type); }
	int  Passengers (void)           { return (passengers); }

	void Vehicle (int value)         { ID (value); }
	void Household (int value)       { household = value; }
	void Location (int value)        { location = value; }
	void Type (int value)            { type = (unsigned char) value; }
	void Sub_Type (int value)        { sub_type = (unsigned char) value; }
	void Passengers (int value)      { passengers = (short) value; }

private:
	int           household;
	int           location;
	short         passengers;
	unsigned char type;
	unsigned char sub_type;
};

//---------------------------------------------------------
//	Vehicle_Array class definition
//---------------------------------------------------------

class Vehicle_Array : public Class_Array
{
public:
	Vehicle_Array (int max_records = 0);

	bool Add (Vehicle_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Vehicle_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Vehicle_Data *) Class_Array::New_Record (clear, number)); }

	Vehicle_Data * Record (int index)       { return ((Vehicle_Data *) Class_Array::Record (index)); }
	Vehicle_Data * Record (void)            { return ((Vehicle_Data *) Class_Array::Record ()); }

	Vehicle_Data * Get (int vehicle)        { return ((Vehicle_Data *) Class_Array::Get (vehicle)); }

	Vehicle_Data * First (void)             { return ((Vehicle_Data *) Class_Array::First ()); }
	Vehicle_Data * Next (void)              { return ((Vehicle_Data *) Class_Array::Next ()); }

	Vehicle_Data * First_Key (void)         { return ((Vehicle_Data *) Class_Array::First_Key ()); }
	Vehicle_Data * Next_Key (void)          { return ((Vehicle_Data *) Class_Array::Next_Key ()); }

	Vehicle_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
