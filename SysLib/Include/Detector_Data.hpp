//*********************************************************
//	Detector_Data.hpp - network detector data
//*********************************************************

#ifndef DETECTOR_DATA_HPP
#define DETECTOR_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Detector_Data class definition
//---------------------------------------------------------

class Detector_Data : public Class_Index
{
public:
	Detector_Data (int id = 0);
	virtual ~Detector_Data (void) {}

	int  Link_Dir (void)           { return (link_dir); }
	int  Link (void)               { return ((link_dir >> 1)); }
	int  Dir (void)                { return ((link_dir & 1)); }
	int  Offset (void)             { return (offset); }
	int  Length (void)             { return (length); }
	int  Low_Lane (void)           { return (low_lane); }
	int  High_Lane (void)          { return (high_lane); }
	int  Type (void)               { return (type); }
	int  Coordinator (void)        { return (coordinator); }

	void Link_Dir (int value)      { link_dir = value; }
	void Link (int value)          { link_dir = (value << 1) + Dir (); }
	void Dir (int value)           { link_dir = (Link () << 1) + value; }
	void Offset (int value)        { offset = value; }
	void Length (int value)        { length = (short) value; }
	void Low_Lane (int value)      { low_lane = (short) value; }
	void High_Lane (int value)     { high_lane = (short) value; }
	void Type (int value)          { type = (short) value; }
	void Coordinator (int value)   { coordinator = value; }

	//---- overloaded methods ----

	int  Low_Cell (void)           { return (offset); }
	void Low_Cell (int value)      { offset = value; }

	int  High_Cell (void)          { return (length); }
	void High_Cell (int value)     { length = (short) value; }

private:
	int   link_dir;
	int   offset;		//---- rounded ----
	short length;		//---- rounded ----
	short low_lane;
	short high_lane;
	short type;
	int   coordinator;
};

//---------------------------------------------------------
//	Detector_Array class definition
//---------------------------------------------------------

class Detector_Array : public Class_Array
{
public:
	Detector_Array (int max_records = 0);

	bool Add (Detector_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Detector_Data * New_Record (bool clear = false, int number = 1)
                                   { return ((Detector_Data *) Class_Array::New_Record (clear, number)); }

	Detector_Data * Record (int index)     { return ((Detector_Data *) Class_Array::Record (index)); }
	Detector_Data * Record (void)          { return ((Detector_Data *) Class_Array::Record ()); }

	Detector_Data * Get (int id)           { return ((Detector_Data *) Class_Array::Get (id)); }
	Detector_Data * Get_GE (int id)        { return ((Detector_Data *) Class_Array::Get_GE (id)); }
	Detector_Data * Get_LE (int id)        { return ((Detector_Data *) Class_Array::Get_LE (id)); }

	Detector_Data * First (void)           { return ((Detector_Data *) Class_Array::First ()); }
	Detector_Data * Next (void)            { return ((Detector_Data *) Class_Array::Next ()); }
	
	Detector_Data * Last (void)            { return ((Detector_Data *) Class_Array::Last ()); }
	Detector_Data * Previous (void)        { return ((Detector_Data *) Class_Array::Previous ()); }

	Detector_Data * First_Key (void)       { return ((Detector_Data *) Class_Array::First_Key ()); }
	Detector_Data * Next_Key (void)        { return ((Detector_Data *) Class_Array::Next_Key ()); }

	Detector_Data * operator[] (int index) { return (Record (index)); }
};

#endif
