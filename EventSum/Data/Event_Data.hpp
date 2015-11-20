//*********************************************************
//	Event_Data.hpp - trip event data
//*********************************************************

#ifndef EVENT_DATA_HPP
#define EVENT_DATA_HPP

#include "Static_Service.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Event_Data class definition
//---------------------------------------------------------

class Event_Data : public Class2_Index, public Static_Scale
{
public:
	Event_Data (void);

	int   Traveler (void)          { return (Key1 ()); }
	int   Household (void)         { return (Key1 () / Traveler_Scale ()); }
	int   Person (void)            { return (Key1 () % Traveler_Scale ()); }
	int   Trip (void)              { return (Key2 () >> 8); }
	int   Leg (void)               { return (Key2 () & 0x00FF); }
	int   Mode (void)              { return (mode); }
	int   Purpose (void)           { return (purpose); }
	int   Constraint (void)        { return (constraint); }
	int   Start_Link (void)        { return (start_link); }
	int   End_Link (void)          { return (end_link); }
	int   Trip_Start (void)        { return (trip_start); }
	int   Trip_End (void)          { return (trip_end); }
	int   Trip_Time (void)         { return (trip_end - trip_start); }
	int   Base_Start (void)        { return (base_start); }
	int   Base_End (void)          { return (base_end); }
	int   Base_Time (void)         { return (base_end - base_start); }
	int   Started (void)           { return (started); }
	int   Ended (void)             { return (ended); }
	int   Base_Start_Diff (void)   { return (started - base_start); }
	int   Base_End_Diff (void)     { return (ended - base_end); }
	int   Trip_Start_Diff (void)   { return (started - trip_start); }
	int   Trip_End_Diff (void)     { return (ended - trip_end); }
	int   Mid_Trip (void)          { return ((started + ended) / 2); }
	int   Travel_Time (void)       { return (ended - started); }
	bool  Select_Flag (void)       { return (select == 1); }

	void  Traveler (int value)     { Key1 (value); }
	void  Household (int value)    { Key1 (value * Traveler_Scale () + Person ()); }
	void  Person (int value)       { Key1 (Household () * Traveler_Scale () + value); }
	void  Trip (int value)         { Key2 ((int) (value << 8) + Leg ()); }
	void  Leg (int value)          { Key2 ((int) ((Key2 () & 0xFFFFFF00) + value)); }
	void  Mode (int value)         { mode = (char) value; }
	void  Purpose (int value)      { purpose = (char) value; }
	void  Constraint (int value)   { constraint = (char) value; }
	void  Start_Link (int value)   { start_link = value; }
	void  End_Link (int value)     { end_link = value; }
	void  Trip_Start (int value)   { trip_start = value; }
	void  Trip_End (int value)     { trip_end = value; }
	void  Base_Start (int value)   { base_start = value; }
	void  Base_End (int value)     { base_end = value; }
	void  Started (int value)      { started = value; }
	void  Ended (int value)        { ended = value; }
	void  Select_Flag (bool flag)  { select = (flag) ? 1 : 0; }

private:
	char  mode, purpose, constraint, select;
	int   start_link, end_link;
	int   trip_start, trip_end, base_start, base_end, started, ended;
};

//---------------------------------------------------------
//	Event_Array class definition
//---------------------------------------------------------

class Event_Array : public Class2_Array
{
public:
	Event_Array (int max_records = 0);

	bool Add (Event_Data *data = NULL)     { return (Class2_Array::Add (data)); }
	
	Event_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((Event_Data *) Class2_Array::New_Record (clear, number)); }

	Event_Data * Get (Event_Data *data)    { return ((Event_Data *) Class2_Array::Get (data)); }
	Event_Data * Get_GE (Event_Data *data) { return ((Event_Data *) Class2_Array::Get_GE (data)); }

	Event_Data * Record (int index)        { return ((Event_Data *) Class2_Array::Record (index)); }
	Event_Data * Record (void)             { return ((Event_Data *) Class2_Array::Record ()); }

	Event_Data * First (void)              { return ((Event_Data *) Class2_Array::First ()); }
	Event_Data * Next (void)               { return ((Event_Data *) Class2_Array::Next ()); }
	
	Event_Data * First_Key (void)          { return ((Event_Data *) Class2_Array::First_Key ()); }
	Event_Data * Next_Key (void)           { return ((Event_Data *) Class2_Array::Next_Key ()); }

	Event_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
