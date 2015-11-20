//*********************************************************
//	Travel_Plan_Data.hpp - travel plan data classes
//*********************************************************

#ifndef TRAVEL_PLAN_DATA_HPP
#define TRAVEL_PLAN_DATA_HPP

#include "Travel_Leg_Data.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	Travel_Plan_Data class definition
//---------------------------------------------------------

class Travel_Plan_Data
{
public:
	Travel_Plan_Data (void);
	virtual ~Travel_Plan_Data (void)         { Clear (); }

	int  Traveler (void)                     { return (traveler); }
	int  Vehicle (void)                      { return (vehicle); }
	int  Start_Time (void)                   { return (start_time); }
	int  End_Time (void)                     { return (end_time); }
	int  Start_Index (void)                  { return (start_index); }
	int  End_Index (void)                    { return (end_index); }
	int  Start_Type (void)                   { return (start_type); }
	int  End_Type (void)                     { return (end_type); }
	int  Mode (void)                         { return (mode); }
	int  Trip (void)                         { return (trip); }
	int  Leg (void)                          { return (leg); }
	int  Start_Offset (void)                 { return (start_offset); }
	int  End_Offset (void)                   { return (end_offset); }
	int  Start_Cell (void)                   { return (start_cell); }
	int  End_Cell (void)                     { return (end_cell); }

	void Traveler (int value)                { traveler = value; }
	void Vehicle (int value)                 { vehicle = value; }
	void Start_Time (int value)              { start_time = value; }
	void End_Time (int value)                { end_time = value; }
	void Start_Index (int value)             { start_index= value; }
	void End_Index (int value)               { end_index = value; }
	void Start_Type (int value)              { start_type = (char) value; }
	void End_Type (int value)                { end_type = (char) value; }
	void Mode (int value)                    { mode = (char) value; }
	void Trip (int value)                    { trip = (char) value; }
	void Leg (int value)                     { leg = (char) value; }
	void Start_Offset (int value)            { start_offset = (short) value; }
	void End_Offset (int value)              { end_offset = (short) value; }
	void Start_Cell (int value)              { start_cell = (short) value; }
	void End_Cell (int value)                { end_cell = (short) value; }

	bool Num_Travel_Legs (int value)         { return (seg.Max_Records (value)); }
	int  Num_Travel_Legs (void)              { return (seg.Num_Records ()); }

	Travel_Leg_Data * Travel_Leg (int index) { return (seg [index]); }
	bool Travel_Leg (Travel_Leg_Data *value) { return (seg.Add (value)); }

	void Clear (void)                        { seg.Clear (); }

private:
	int   traveler;			//---- traveler/line index ----
	int   vehicle;			//---- vehicle/line index ----
	int   start_time;		//---- time step ----
	int   end_time;			//---- time step ----
	int   start_index;
	int   end_index;
	char  start_type :8;
	char  end_type   :8;
	char  mode;
	char  trip;
	char  leg;
	short start_cell;		//---- link cell ----
	short end_cell;			//---- link cell ----
	short start_offset;		//---- cell offset ----
	short end_offset;		//---- cell offset ----

	Travel_Leg_Array seg;
};

//---------------------------------------------------------
//	Travel_Plan_Array
//---------------------------------------------------------

class Travel_Plan_Array : public Data_Array
{
public:
	Travel_Plan_Array (int max_records = 0);
	virtual ~Travel_Plan_Array (void);

	bool Add (Travel_Plan_Data *data)			{ return (Data_Array::Add ((void *) data)); }

	Travel_Plan_Data * Record (int index)       { return ((Travel_Plan_Data *) Data_Array::Record (index)); }
	Travel_Plan_Data * Record (void)            { return ((Travel_Plan_Data *) Data_Array::Record ()); }

	Travel_Plan_Data * First (void)				{ return ((Travel_Plan_Data *) Data_Array::First ()); }
	Travel_Plan_Data * Next (void)				{ return ((Travel_Plan_Data *) Data_Array::Next ()); }

	Travel_Plan_Data * Last (void)				{ return ((Travel_Plan_Data *) Data_Array::Last ()); }
	Travel_Plan_Data * Previous (void)			{ return ((Travel_Plan_Data *) Data_Array::Previous ()); }

	Travel_Plan_Data * operator[] (int index)	{ return (Record (index)); }

	Travel_Plan_Data * Copy (int index);
};

//---------------------------------------------------------
//	Travel_Plan_Queue
//---------------------------------------------------------

class Travel_Plan_Queue : public Data_Queue
{
public:
	Travel_Plan_Queue (int max_records = 0);

	bool Add (Travel_Plan_Data *data)  { return (Data_Queue::Add ((void *) data)); }

	Travel_Plan_Data * First (void)    { return ((Travel_Plan_Data *) Data_Queue::First ()); }
	Travel_Plan_Data * Next (void)     { return ((Travel_Plan_Data *) Data_Queue::Next ()); }

	virtual void Remove (bool delete_flag = true);
};

#endif
