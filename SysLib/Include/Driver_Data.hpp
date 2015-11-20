//*********************************************************
//	Driver_Data.hpp - transit driver classes
//*********************************************************

#ifndef DRIVER_DATA_HPP
#define DRIVER_DATA_HPP

#include "Class_Array.hpp"

#include <string.h>

//---------------------------------------------------------
//	Driver_Data class definition
//---------------------------------------------------------

class Driver_Data : public Class_Index
{
public:
	Driver_Data (int id = 0);
	virtual ~Driver_Data (void)        { Clear (); }

	int  Route (void)                  { return (ID ()); }
	int  Type (void)                   { return (type); }
	int  Sub_Type (void)               { return (sub_type); }
	int  Links (void)                  { return (nlinks); }
	int  Link_Dir (int num)            { return (Index (num) ? link [num-1] : 0); }
	int  Link (int num)                { return (Link_Dir (num) >> 1); }
	int  Dir (int num)                 { return (Link_Dir (num) & 1); }

	void Route (int value)             { ID (value); }
	void Type (int value)              { type = (short) value; }
	void Sub_Type (int value)          { sub_type = (short) value; }
	bool Links (int value);
	void Link_Dir (int num, int value) { if (Index (num)) link [num-1] = value; }
	void Link (int num, int value)     { Link_Dir (num, ((value << 1) + Dir (num))); }
	void Dir (int num, int value)      { Link_Dir (num, ((Link (num) << 1) + value)); }
	
	void   Null_Notes (void)           { notes = NULL; }
	const char * Notes (void)          { return ((notes != NULL) ? notes : ""); }
	bool   Notes (char *value);

	void Clear (void);

private:

	short type;
	short sub_type;
	int   nlinks;
	char  *notes;
	int   *link;

	bool Index (int num)               { return (num > 0 && num <= nlinks); }
};

//---------------------------------------------------------
//	Driver_Array class definition
//---------------------------------------------------------

class Driver_Array : public Class_Array
{
public:
	Driver_Array (int max_records = 0);
	virtual ~Driver_Array (void);

	bool Add (Driver_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Driver_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Driver_Data *) Class_Array::New_Record (clear, number)); }

	Driver_Data * Record (int index)     { return ((Driver_Data *) Class_Array::Record (index)); }
	Driver_Data * Record (void)          { return ((Driver_Data *) Class_Array::Record ()); }

	Driver_Data * Get (int id)           { return ((Driver_Data *) Class_Array::Get (id)); }
	Driver_Data * Get_GE (int id)        { return ((Driver_Data *) Class_Array::Get_GE (id)); }
	Driver_Data * Get_LE (int id)        { return ((Driver_Data *) Class_Array::Get_LE (id)); }

	Driver_Data * First (void)           { return ((Driver_Data *) Class_Array::First ()); }
	Driver_Data * Next (void)            { return ((Driver_Data *) Class_Array::Next ()); }
	
	Driver_Data * Last (void)            { return ((Driver_Data *) Class_Array::Last ()); }
	Driver_Data * Previous (void)        { return ((Driver_Data *) Class_Array::Previous ()); }

	Driver_Data * First_Key (void)       { return ((Driver_Data *) Class_Array::First_Key ()); }
	Driver_Data * Next_Key (void)        { return ((Driver_Data *) Class_Array::Next_Key ()); }

	Driver_Data * operator[] (int index) { return (Record (index)); }

	int  Driver_Records (void)         { return (driver_records); }
	void Driver_Records (int value)    { driver_records = value; }

private:
	int driver_records;
};

#endif
