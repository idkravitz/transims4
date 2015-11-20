//*********************************************************
//	Toll_Data.hpp - network toll data
//*********************************************************

#ifndef TOLL_DATA_HPP
#define TOLL_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Toll_Data class definition
//---------------------------------------------------------

class Toll_Data : public Class_Index
{
public:
	Toll_Data (int link_dir = 0);
	virtual ~Toll_Data (void)     {}

	int  Link_Dir (void)              { return (ID ()); }
	int  Link (void)                  { return ((ID () >> 1)); }
	int  Dir (void)                   { return ((ID () & 1)); }
	int  Start (void)                 { return (start); }
	int  End (void)                   { return (end); }
	int  Use (void)                   { return (use); }
	int  Toll (void)                  { return (toll); }
	int  TOD_List (void)              { return (list); }
	
	void Link_Dir (int value)         { ID (value); }
	void Link (int value)             { ID ((value << 1) + Dir ()); }
	void Dir (int value)              { ID ((Link () << 1) + value); }
	void Start (int value)            { start = value; }
	void End (int value)              { end = value; }
	void Use (int value)              { use = (unsigned short) value; }
	void Toll (int value)             { toll = (unsigned short) value; }
	void TOD_List (int value)         { list = value; }

private:
	int            start;			//---- rounded ----
	int            end;				//---- rounded ----
	unsigned short use;
	unsigned short toll;			//---- cents ----
	int            list;
};

//---------------------------------------------------------
//	Toll_Array class definition
//---------------------------------------------------------

class Toll_Array : public Class_Array
{
public:
	Toll_Array (int max_records = 0);

	bool Add (Toll_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Toll_Data * New_Record (bool clear = false, int number = 1)
	                                   { return ((Toll_Data *) Class_Array::New_Record (clear, number)); }

	Toll_Data * Record (int index)     { return ((Toll_Data *) Class_Array::Record (index)); }
	Toll_Data * Record (void)          { return ((Toll_Data *) Class_Array::Record ()); }

	Toll_Data * Get (int link)         { return ((Toll_Data *) Class_Array::Get (link)); }
	Toll_Data * Get_GE (int link)      { return ((Toll_Data *) Class_Array::Get_GE (link)); }
	Toll_Data * Get_LE (int link)      { return ((Toll_Data *) Class_Array::Get_LE (link)); }

	Toll_Data * First (void)           { return ((Toll_Data *) Class_Array::First ()); }
	Toll_Data * Next (void)            { return ((Toll_Data *) Class_Array::Next ()); }
	
	Toll_Data * Last (void)            { return ((Toll_Data *) Class_Array::Last ()); }
	Toll_Data * Previous (void)        { return ((Toll_Data *) Class_Array::Previous ()); }

	Toll_Data * First_Key (void)       { return ((Toll_Data *) Class_Array::First_Key ()); }
	Toll_Data * Next_Key (void)        { return ((Toll_Data *) Class_Array::Next_Key ()); }

	Toll_Data * operator[] (int index) { return (Record (index)); }
};

#endif
