//*********************************************************
//	Activity_Data.hpp - activity data classes
//*********************************************************

#ifndef ACTIVITY_DATA_HPP
#define ACTIVITY_DATA_HPP

#include "Static_Service.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Activity_Data class definition
//---------------------------------------------------------

class Activity_Data : public Class2_Index, public Static_Scale
{
public:
	Activity_Data (int traveler = 0, int activity = 0);
	virtual ~Activity_Data (void)   {}

	int  Traveler (void)          { return (Key1 ()); }
	int  Household (void)         { return (Key1 () / Traveler_Scale ()); }
	int  Person (void)            { return (Key1 () % Traveler_Scale ()); }
	int  Activity (void)          { return (Key2 ()); }
	int  Purpose (void)           { return (purpose); }
	int  Priority (void)          { return (priority); }
	int  Start_Time (void)        { return (start_time); }
	int  End_Time (void)          { return (end_time); }
	int  Duration (void)          { return (duration); }
	int  Mode (void)              { return (mode); }
	int  Vehicle (void)           { return (vehicle); }
	int  Location (void)          { return (location); }
	int  Passengers (void)        { return (passengers); }
	int  Constraint (void)        { return (constraint); }
	int  Problem (void)           { return (problem); }

	void Traveler (int value)     { Key1 (value); }
	void Household (int value)    { Key1 (value * Traveler_Scale () + Person ()); }
	void Person (int value)       { Key1 (Household () * Traveler_Scale () + value); }
	void Activity (int value)     { Key2 (value); }
	void Purpose (int value)      { purpose = (char) value; }
	void Priority (int value)     { priority = (char) value; }
	void Start_Time (int value)   { start_time = value; }
	void End_Time (int value)     { end_time = value; }
	void Duration (int value)     { duration = value; }
	void Mode (int value)         { mode = (char) value; }
	void Vehicle (int value)      { vehicle = value; }
	void Location (int value)     { location = value; }
	void Passengers (int value)   { passengers = (short) value; }
	void Constraint (int value)   { constraint = (char) value; }
	void Problem (int value)      { problem = (short) value; }

private:
	int   start_time;
	int   end_time;
	int   duration;
	int   location;
	int   vehicle;
	char  purpose;
	char  priority;
	char  mode;
	char  constraint;
	short passengers;
	short problem;
};

//---------------------------------------------------------
//	Activity_Array class definition
//---------------------------------------------------------

class Activity_Array : public Class2_Array
{
public:
	Activity_Array (int max_records = 0);

	bool Add (Activity_Data *data = NULL)        { return (Class2_Array::Add (data)); }
	
	Activity_Data * New_Record (bool clear = false, int number = 1)
 	                                             { return ((Activity_Data *) Class2_Array::New_Record (clear, number)); }

	Activity_Data * Record (int index)           { return ((Activity_Data *) Class2_Array::Record (index)); }
	Activity_Data * Record (void)                { return ((Activity_Data *) Class2_Array::Record ()); }

	Activity_Data * Get (int key1, int key2)     { return ((Activity_Data *) Class2_Array::Get (key1, key2)); }
	Activity_Data * Get (Activity_Data *data)    { return ((Activity_Data *) Class2_Array::Get (data)); }

	Activity_Data * Get_GE (int key1, int key2)  { return ((Activity_Data *) Class2_Array::Get_GE (key1, key2)); }
	Activity_Data * Get_GE (Activity_Data *data) { return ((Activity_Data *) Class2_Array::Get_GE (data)); }

	Activity_Data * Get_LE (int key1, int key2)  { return ((Activity_Data *) Class2_Array::Get_LE (key1, key2)); }
	Activity_Data * Get_LE (Activity_Data *data) { return ((Activity_Data *) Class2_Array::Get_LE (data)); }

	Activity_Data * First (void)                 { return ((Activity_Data *) Class2_Array::First ()); }
	Activity_Data * Next (void)                  { return ((Activity_Data *) Class2_Array::Next ()); }
	
	Activity_Data * Last (void)                  { return ((Activity_Data *) Class2_Array::Last ()); }
	Activity_Data * Previous (void)              { return ((Activity_Data *) Class2_Array::Previous ()); }

	Activity_Data * First_Key (void)             { return ((Activity_Data *) Class2_Array::First_Key ()); }
	Activity_Data * Next_Key (void)              { return ((Activity_Data *) Class2_Array::Next_Key ()); }

	Activity_Data * Last_Key (void)              { return ((Activity_Data *) Class2_Array::Last_Key ()); }
	Activity_Data * Previous_Key (void)          { return ((Activity_Data *) Class2_Array::Previous_Key ()); }

	Activity_Data * operator[] (int index)       { return (Record (index)); }
};

#endif

