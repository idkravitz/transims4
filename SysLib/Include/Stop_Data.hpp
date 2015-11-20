//*********************************************************
//	Stop_Data.hpp - network transit stop data
//*********************************************************

#ifndef STOP_DATA_HPP
#define STOP_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Stop_Data class definition
//---------------------------------------------------------

class Stop_Data : public Class_Index
{
public:
	Stop_Data (int id = 0);
	virtual ~Stop_Data (void)  {}

	int  Stop (void)           { return (ID ()); }
	int  Link_Dir (void)       { return (link_dir); }
	int  Link (void)           { return ((link_dir >> 1)); }
	int  Dir (void)            { return ((link_dir & 1)); } 
	int  Offset (void)         { return (offset); }
	int  Use (void)	           { return (use); }
	int  Type (void)           { return (type); }
	int  Space (void)          { return (space); }
	int  Traveler (void)       { return (traveler); }

	void Stop (int value)      { ID (value); }
	void Link_Dir (int value)  { link_dir = value; }
	void Link (int value)      { link_dir = (value << 1) + Dir (); }
	void Dir (int value)       { link_dir = (Link () << 1) + value; }
	void Offset (int value)    { offset = value; }
	void Use (int value)       { use = (unsigned short) value; }
	void Type (int value)      { type = (unsigned char) value; }
	void Space (int value)     { space = (unsigned char) value; }
	void Traveler (int value)  { traveler = value; }

	void   Null_Name (void)    { name = NULL; }
	const char * Name (void)   { return ((name != NULL) ? name : ""); }
	bool   Name (char *value);
	
	void   Null_Notes (void)   { notes = NULL; }
	const char * Notes (void)  { return ((notes != NULL) ? notes : ""); }
	bool   Notes (char *value);

	void  Clear (void);

	//---- overloaded methods ----

	int  Cell (void)           { return (offset); }
	void Cell (int value)      { offset = value; }

	int  Num_Runs (void)       { return (traveler); }
	void Num_Runs (int value)  { traveler = value; }
	void Add_Runs (int value)  { traveler += value; }

private:
	int            link_dir;
	int            offset;		//---- rounded ----
	unsigned short use;	
	unsigned char  type;
	unsigned char  space;
	int            traveler;
	char *         name;
	char *         notes;
};

//---------------------------------------------------------
//	Stop_Array class definition
//---------------------------------------------------------

class Stop_Array : public Class_Array
{
public:
	Stop_Array (int max_records = 0);
	virtual ~Stop_Array (void);

	bool Add (Stop_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Stop_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Stop_Data *) Class_Array::New_Record (clear, number)); }

	Stop_Data * Record (int index)       { return ((Stop_Data *) Class_Array::Record (index)); }
	Stop_Data * Record (void)            { return ((Stop_Data *) Class_Array::Record ()); }

	Stop_Data * Get (int id)             { return ((Stop_Data *) Class_Array::Get (id)); }
	Stop_Data * Get_GE (int id)          { return ((Stop_Data *) Class_Array::Get_GE (id)); }
	Stop_Data * Get_LE (int id)          { return ((Stop_Data *) Class_Array::Get_LE (id)); }

	Stop_Data * First (void)             { return ((Stop_Data *) Class_Array::First ()); }
	Stop_Data * Next (void)              { return ((Stop_Data *) Class_Array::Next ()); }
	
	Stop_Data * Last (void)              { return ((Stop_Data *) Class_Array::Last ()); }
	Stop_Data * Previous (void)          { return ((Stop_Data *) Class_Array::Previous ()); }

	Stop_Data * First_Key (void)         { return ((Stop_Data *) Class_Array::First_Key ()); }
	Stop_Data * Next_Key (void)          { return ((Stop_Data *) Class_Array::Next_Key ()); }

	Stop_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
