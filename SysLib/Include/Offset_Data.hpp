//*********************************************************
//	Offset_Data.hpp - link offset data
//*********************************************************

#ifndef OFFSET_DATA_HPP
#define OFFSET_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Offset_Data class definition
//---------------------------------------------------------

class Offset_Data : public Class_Index
{
public:
	Offset_Data (int id = 0);

	int    Link_Dir (void)     { return (link_dir); }
	int    Link (void)         { return ((link_dir >> 1)); }
	int    Dir (void)          { return ((link_dir & 1)); }
	double Offset (void)       { return (offset); }
	double X (void)            { return (x); }
	double Y (void)            { return (y); }
	double Z (void)            { return (z); }


	void Link_Dir (int value)  { link_dir = value; }
	void Link (int value)      { link_dir = (value << 1) + Dir (); }
	void Dir (int value)       { link_dir = (Link () << 1) + value; }
	void Offset (double value) { offset = value; }
	void X (double value)      { x = value; }
	void Y (double value)      { y = value; }
	void Z (double value)      { z = value; }

private:
	int id;
	int link_dir;
	double offset;
	double x;
	double y;
	double z;
};

//---------------------------------------------------------
//	Offset_Array class definition
//---------------------------------------------------------

class Offset_Array : public Class_Array
{
public:
	Offset_Array (int max_records = 0);

	bool Add (Offset_Data *data = NULL)      { return (Class_Array::Add (data)); }
	
	Offset_Data * Record (int index)         { return ((Offset_Data *) Class_Array::Record (index)); }
	Offset_Data * Record (void)              { return ((Offset_Data *) Class_Array::Record ()); }

	Offset_Data * Get (int id)               { return ((Offset_Data *) Class_Array::Get (id)); }

	Offset_Data * First (void)               { return ((Offset_Data *) Class_Array::First ()); }
	Offset_Data * Next (void)                { return ((Offset_Data *) Class_Array::Next ()); }

	Offset_Data * First_Key (void)           { return ((Offset_Data *) Class_Array::First_Key ()); }
	Offset_Data * Next_Key (void)            { return ((Offset_Data *) Class_Array::Next_Key ()); }

	Offset_Data * operator[] (int index)     { return (Record (index)); }
};

#endif
