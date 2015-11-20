//*********************************************************
//	Parking_Data.hpp - network parking data
//*********************************************************

#ifndef PARKING_DATA_HPP
#define PARKING_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Parking_Data class definition
//---------------------------------------------------------

class Parking_Data : public Class_Index
{
public:
	Parking_Data (int id = 0);
	virtual ~Parking_Data (void) {}

	int  Link_Dir (void)         { return (link_dir); }
	int  Link (void)             { return ((link_dir >> 1)); }
	int  Dir (void)              { return ((link_dir & 1)); }
	int  Offset (void)           { return (offset); }
	int  Type (void)             { return (type); }
	int  Use (void)              { return (use); }
	int  Hourly (void)           { return (hourly); }
	int  Daily (void)            { return (daily); }
	int  Space (void)            { return (space); }
	int  Start (void)            { return (start); }
	int  End (void)              { return (end); }

	void Link_Dir (int value)    { link_dir = value; }
	void Link (int value)        { link_dir = (value << 1) + Dir (); }
	void Dir (int value)         { link_dir = (Link () << 1) + value; }
	void Offset (int value)      { offset = value; }
	void Type (int value)        { type = (unsigned short) value; }
	void Use (int value)         { use = (unsigned short) value; }
	void Hourly (int value)      { hourly = (unsigned short) value; }
	void Daily (int value)       { daily = (unsigned short) value; }
	void Space (int value)       { space = value; }
	void Start (int value)       { start = value; }
	void End (int value)         { end = value; }

	//---- overloaded method ----

	int  Penalty (void)          { return (space); }
	void Penalty (int value)     { space = value; }

private:
	int            link_dir;
	int            offset;		//---- rounded ----
	unsigned short type;
	unsigned short use;
	unsigned short hourly;
	unsigned short daily;
	int            space;
	int            start;		//---- rounded ----
	int            end;			//---- rounded ----
};

//---------------------------------------------------------
//	Parking_Array class definition
//---------------------------------------------------------

class Parking_Array : public Class_Array
{
public:
	Parking_Array (int max_records = 0);

	bool Add (Parking_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Parking_Data * New_Record (bool clear = false, int number = 1)
	                                      { return ((Parking_Data *) Class_Array::New_Record (clear, number)); }

	Parking_Data * Record (int index)     { return ((Parking_Data *) Class_Array::Record (index)); }
	Parking_Data * Record (void)          { return ((Parking_Data *) Class_Array::Record ()); }

	Parking_Data * Get (int id)           { return ((Parking_Data *) Class_Array::Get (id)); }
	Parking_Data * Get_GE (int id)        { return ((Parking_Data *) Class_Array::Get_GE (id)); }
	Parking_Data * Get_LE (int id)        { return ((Parking_Data *) Class_Array::Get_LE (id)); }

	Parking_Data * First (void)           { return ((Parking_Data *) Class_Array::First ()); }
	Parking_Data * Next (void)            { return ((Parking_Data *) Class_Array::Next ()); }
	
	Parking_Data * Last (void)            { return ((Parking_Data *) Class_Array::Last ()); }
	Parking_Data * Previous (void)        { return ((Parking_Data *) Class_Array::Previous ()); }

	Parking_Data * First_Key (void)       { return ((Parking_Data *) Class_Array::First_Key ()); }
	Parking_Data * Next_Key (void)        { return ((Parking_Data *) Class_Array::Next_Key ()); }

	Parking_Data * operator[] (int index) { return (Record (index)); }
};

#endif
