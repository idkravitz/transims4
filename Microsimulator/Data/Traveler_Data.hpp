//*********************************************************
//	Traveler_Data.hpp - traveler data classes
//*********************************************************

#ifndef TRAVELER_DATA_HPP
#define TRAVELER_DATA_HPP

#include "Class_Array.hpp"
#include "Travel_Plan_Data.hpp"

//---------------------------------------------------------
//	Traveler_Data class definition
//---------------------------------------------------------

class Traveler_Data : public Class_Index
{
public:
	Traveler_Data (int traveler = 0);

	int  Traveler (void)             { return (ID ()); }
	bool Active (void)               { return (active); }
	bool Problem (void)              { return (problem); }
	int  End_Time (void)             { return (end_time); }
	int  Next_Rider (void)           { return (next_rider); }

	void Traveler (int value)        { ID (value); }
	void Active (bool value)         { active = value; }
	void Problem (bool value)        { problem = value; }
	void End_Time (int value)        { end_time = value; }
	void Next_Rider (int value)      { next_rider = value; }
	
	Travel_Plan_Data * Travel_Plan (void)     { return (plan_ptr); }
	void Travel_Plan (Travel_Plan_Data *ptr)  { plan_ptr = ptr; }

private:
	bool active;
	bool problem;
	int  end_time;		//---- time step ----
	int  next_rider;

	Travel_Plan_Data *plan_ptr;
};

//---------------------------------------------------------
//	Traveler_Array class definition
//---------------------------------------------------------

class Traveler_Array : public Class_Array
{
public:
	Traveler_Array (int max_records = 0);

	bool Add (Traveler_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Traveler_Data * Record (int index)       { return ((Traveler_Data *) Class_Array::Record (index)); }
	Traveler_Data * Record (void)            { return ((Traveler_Data *) Class_Array::Record ()); }

	Traveler_Data * Get (int traveler)       { return ((Traveler_Data *) Class_Array::Get (traveler)); }

	Traveler_Data * First (void)             { return ((Traveler_Data *) Class_Array::First ()); }
	Traveler_Data * Next (void)              { return ((Traveler_Data *) Class_Array::Next ()); }

	Traveler_Data * operator[] (int index)	 { return (Record (index)); }
};

#endif
