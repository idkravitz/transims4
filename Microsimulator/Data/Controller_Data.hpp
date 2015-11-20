//*********************************************************
//	Controller_Data.hpp - traffic controller data classes
//*********************************************************

#ifndef CONTROLLER_DATA_HPP
#define CONTROLLER_DATA_HPP

#include "Class_Array.hpp"
#include "Network_Code.hpp"

//---------------------------------------------------------
//	Controller_Data class definition
//---------------------------------------------------------

class Controller_Data : public Class_Index
{
public:
	Controller_Data (int node = 0);

	int  Node (void)                   { return (ID ()); }
	int  End_Time (void)               { return (end_time); }
	int  Type (void)                   { return (type); }
	int  Rings (void)                  { return (rings); }
	int  Group (void)                  { return (group); }
	int  Groups (void)                 { return (groups); }
	int  Phase (int num)               { return (Index (num) ? phase [num-1] : 0); }
	int  Start (int num)               { return (Index (num) ? start [num-1] : 0); }
	int  Check (int num)               { return (Index (num) ? check [num-1] : 0); }
	int  Status (int num)              { return (Index (num) ? status [num-1] : 0); }
	int  Min_Time (int num)            { return ((num > 0 && num <= 8) ? min_time [num-1] : 0); }
	int  Movement (void)               { return (movement); }

	void Node (int value)              { ID (value); }
	void End_Time (int value)          { end_time = value; }
	void Type (int value)              { type = (char) value; }
	void Rings (int value)             { rings = (char) value; }
	void Group (int value)             { group = (char) value; }
	void Groups (int value)            { groups = (char) value; }
	void Phase (int num, int value)    { if (Index (num)) phase [num-1] = value; }
	void Start (int num, int value)    { if (Index (num)) start [num-1] = value; }
	void Check (int num, int value)    { if (Index (num)) check [num-1] = value; }
	void Status (int num, int value)   { if (Index (num)) status [num-1] = value; }
	void Min_Time (int num, int value) { if (num > 0 && num <= 8) min_time [num-1] = (short) value; }
	void Movement (int value)          { movement = value; }
	
	int  Max_Rings (void)              { return (Network_Code::MAX_RING); }

private:
	int   end_time;
	char  type;
	char  rings;
	char  group;
	char  groups;
	int   phase [Network_Code::MAX_RING];
	int   start [Network_Code::MAX_RING];
	int   check [Network_Code::MAX_RING];
	int   status [Network_Code::MAX_RING];
	short min_time [8];
	int   movement;

	bool Index (int num)             { return (num > 0 && num <= Max_Rings ()); }
};

//---------------------------------------------------------
//	Controller_Array class definition
//---------------------------------------------------------

class Controller_Array : public Class_Array
{
public:
	Controller_Array (int max_records = 0);

	bool Add (Controller_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Controller_Data * Record (int index)       { return ((Controller_Data *) Class_Array::Record (index)); }
	Controller_Data * Record (void)            { return ((Controller_Data *) Class_Array::Record ()); }

	Controller_Data * Get (int node)           { return ((Controller_Data *) Class_Array::Get (node)); }

	Controller_Data * First (void)             { return ((Controller_Data *) Class_Array::First ()); }
	Controller_Data * Next (void)              { return ((Controller_Data *) Class_Array::Next ()); }

	Controller_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
