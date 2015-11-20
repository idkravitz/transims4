//*********************************************************
//	Location_Data.hpp - network activity location data
//*********************************************************

#ifndef LOCATION_DATA_HPP
#define LOCATION_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Location_Data class definition
//---------------------------------------------------------

class Location_Data : public Class_Index
{
public:
	Location_Data (int id = 0);
	virtual ~Location_Data (void) {}

	int  Location (void)          { return (ID ()); }
	int  Link_Dir (void)          { return (link_dir); }
	int  Link (void)              { return ((link_dir >> 1)); }
	int  Dir (void)               { return ((link_dir & 1)); }
	int  Offset (void)            { return (offset); }
	int  X (void)                 { return (x); }
	int  Y (void)                 { return (y); }
	int  Zone (void)              { return (zone); }

	void Location (int value)     { ID (value); }
	void Link_Dir (int value)     { link_dir = value; }
	void Link (int value)         { link_dir = (value << 1) + Dir (); }
	void Dir (int value)          { link_dir = (Link () << 1) + value; }
	void Offset (int value)       { offset = value; }
	void X (int value)            { x = value; }
	void Y (int value)            { y = value; }
	void Zone (int value)         { zone = value; }

	//---- overloaded methods ----

	int  Weight (void)            { return (link_dir); }
	void Weight (int value)       { link_dir = value; }
	void Add_Weight (int value)   { link_dir += value; }

	int  Count (void)             { return (zone); }
	void Count (int value)        { zone = value; }
	void Add_Count (int value)    { zone += value; }

	void Add_Offset (int value)   { offset += value; }

private:
	int   link_dir;
	int   offset;		//---- rounded ----
	int   x;			//---- rounded ----
	int   y;
	int   zone;
};

//---------------------------------------------------------
//	Location_Array class definition
//---------------------------------------------------------

class Location_Array : public Class_Array
{
public:
	Location_Array (int max_records = 0);

	bool Add (Location_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Location_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Location_Data *) Class_Array::New_Record (clear, number)); }

	Location_Data * Record (int index)     { return ((Location_Data *) Class_Array::Record (index)); }
	Location_Data * Record (void)          { return ((Location_Data *) Class_Array::Record ()); }

	Location_Data * Get (int id)           { return ((Location_Data *) Class_Array::Get (id)); }
	Location_Data * Get_GE (int id)        { return ((Location_Data *) Class_Array::Get_GE (id)); }
	Location_Data * Get_LE (int id)        { return ((Location_Data *) Class_Array::Get_LE (id)); }

	Location_Data * First (void)           { return ((Location_Data *) Class_Array::First ()); }
	Location_Data * Next (void)            { return ((Location_Data *) Class_Array::Next ()); }
	
	Location_Data * Last (void)            { return ((Location_Data *) Class_Array::Last ()); }
	Location_Data * Previous (void)        { return ((Location_Data *) Class_Array::Previous ()); }

	Location_Data * First_Key (void)       { return ((Location_Data *) Class_Array::First_Key ()); }
	Location_Data * Next_Key (void)        { return ((Location_Data *) Class_Array::Next_Key ()); }

	Location_Data * operator[] (int index) { return (Record (index)); }
};

#endif
