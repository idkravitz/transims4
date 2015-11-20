//*********************************************************
//	Trip_Data.hpp - trip data classes
//*********************************************************

#ifndef TRIP_DATA_HPP
#define TRIP_DATA_HPP

#include "Static_Service.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Trip_Data class definition
//---------------------------------------------------------

class Trip_Data : public Class2_Index, public Static_Scale
{
public:
	Trip_Data (int traveler = 0, int trip = 0);
	virtual ~Trip_Data (void)   {}

	int  Traveler (void)          { return (Key1 ()); }
	int  Household (void)         { return (Key1 () / Traveler_Scale ()); }
	int  Person (void)            { return (Key1 () % Traveler_Scale ()); }
	int  Trip (void)              { return (Key2 ()); }
	int  Purpose (void)           { return (purpose); }
	int  Priority (void)          { return (priority); }
	int  Start_Time (void)        { return (start_time); }
	int  End_Time (void)          { return (end_time); }
	int  Mode (void)              { return (mode); }
	int  Vehicle (void)           { return (vehicle); }
	int  Origin (void)            { return (origin); }
	int  Destination (void)       { return (destination); }
	int  Constraint (void)        { return (constraint); }

	void Traveler (int value)     { Key1 (value); }
	void Household (int value)    { Key1 (value * Traveler_Scale () + Person ()); }
	void Person (int value)       { Key1 (Household () * Traveler_Scale () + value); }
	void Trip (int value)         { Key2 (value); }
	void Purpose (int value)      { purpose = (char) value; }
	void Priority (int value)     { priority = (char) value; }
	void Start_Time (int value)   { start_time = value; }
	void End_Time (int value)     { end_time = value; }
	void Mode (int value)         { mode = (char) value; }
	void Vehicle (int value)      { vehicle = value; }
	void Origin (int value)       { origin = value; }
	void Destination (int value)  { destination = value; }
	void Constraint (int value)   { constraint = (char) value; }

private:
	int   start_time;
	int   end_time;
	int   origin;
	int   destination;
	int   vehicle;
	char  purpose;
	char  priority;
	char  mode;
	char  constraint;
};

//---------------------------------------------------------
//	Trip_Array class definition
//---------------------------------------------------------

class Trip_Array : public Class2_Array
{
public:
	Trip_Array (int max_records = 0);

	bool Add (Trip_Data *data = NULL)        { return (Class2_Array::Add (data)); }
	
	Trip_Data * New_Record (bool clear = false, int number = 1)
 	                                         { return ((Trip_Data *) Class2_Array::New_Record (clear, number)); }

	Trip_Data * Record (int index)           { return ((Trip_Data *) Class2_Array::Record (index)); }
	Trip_Data * Record (void)                { return ((Trip_Data *) Class2_Array::Record ()); }

	Trip_Data * Get (int key1, int key2)     { return ((Trip_Data *) Class2_Array::Get (key1, key2)); }
	Trip_Data * Get (Trip_Data *data)        { return ((Trip_Data *) Class2_Array::Get (data)); }

	Trip_Data * Get_GE (int key1, int key2)  { return ((Trip_Data *) Class2_Array::Get_GE (key1, key2)); }
	Trip_Data * Get_GE (Trip_Data *data)     { return ((Trip_Data *) Class2_Array::Get_GE (data)); }

	Trip_Data * Get_LE (int key1, int key2)  { return ((Trip_Data *) Class2_Array::Get_LE (key1, key2)); }
	Trip_Data * Get_LE (Trip_Data *data)     { return ((Trip_Data *) Class2_Array::Get_LE (data)); }

	Trip_Data * First (void)                 { return ((Trip_Data *) Class2_Array::First ()); }
	Trip_Data * Next (void)                  { return ((Trip_Data *) Class2_Array::Next ()); }
	
	Trip_Data * Last (void)                  { return ((Trip_Data *) Class2_Array::Last ()); }
	Trip_Data * Previous (void)              { return ((Trip_Data *) Class2_Array::Previous ()); }

	Trip_Data * First_Key (void)             { return ((Trip_Data *) Class2_Array::First_Key ()); }
	Trip_Data * Next_Key (void)              { return ((Trip_Data *) Class2_Array::Next_Key ()); }

	Trip_Data * Last_Key (void)              { return ((Trip_Data *) Class2_Array::Last_Key ()); }
	Trip_Data * Previous_Key (void)          { return ((Trip_Data *) Class2_Array::Previous_Key ()); }

	Trip_Data * operator[] (int index)       { return (Record (index)); }
};

#endif

