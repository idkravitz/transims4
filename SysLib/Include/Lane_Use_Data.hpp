//*********************************************************
//	Lane_Use_Data.hpp - network lane use data
//*********************************************************

#ifndef LANE_USE_DATA_HPP
#define LANE_USE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Lane_Use_Data class definition
//---------------------------------------------------------

class Lane_Use_Data : public Class_Index
{
public:
	Lane_Use_Data (int link_dir = 0);
	virtual ~Lane_Use_Data (void)     {}

	int  Link_Dir (void)              { return (ID ()); }
	int  Link (void)                  { return ((ID () >> 1)); }
	int  Dir (void)                   { return ((ID () & 1)); }
	int  Lane (void)                  { return (lane); }
	int  Type (void)                  { return (type); }
	int  Use (void)                   { return (use); }
	int  Start (void)                 { return (start); }
	int  End (void)                   { return (end); }
	int  Offset (void)                { return (offset); }
	int  Length (void)                { return (length); }
	
	void Link_Dir (int value)         { ID (value); }
	void Link (int value)             { ID ((value << 1) + Dir ()); }
	void Dir (int value)              { ID ((Link () << 1) + value); }
	void Lane (int value)             { lane = (unsigned char) value; }
	void Type (int value)             { type = (unsigned char) value; }
	void Use (int value)              { use = (unsigned short) value; }
	void Start (int value)            { start = value; }
	void End (int value)              { end = value; }
	void Offset (int value)           { offset = value; }
	void Length (int value)           { length = value; }

private:
	unsigned char  lane;
	unsigned char  type;
	unsigned short use;
	int            start;			//---- rounded ----
	int            end;				//---- rounded ----
	int            offset;			//---- rounded ----
	int            length;			//---- rounded ----
};

//---------------------------------------------------------
//	Lane_Use_Array class definition
//---------------------------------------------------------

class Lane_Use_Array : public Class_Array
{
public:
	Lane_Use_Array (int max_records = 0);

	bool Add (Lane_Use_Data *data = NULL)  { return (Class_Array::Add (data)); }
	
	Lane_Use_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((Lane_Use_Data *) Class_Array::New_Record (clear, number)); }

	Lane_Use_Data * Record (int index)     { return ((Lane_Use_Data *) Class_Array::Record (index)); }
	Lane_Use_Data * Record (void)          { return ((Lane_Use_Data *) Class_Array::Record ()); }

	Lane_Use_Data * Get (int link)         { return ((Lane_Use_Data *) Class_Array::Get (link)); }
	Lane_Use_Data * Get_GE (int link)      { return ((Lane_Use_Data *) Class_Array::Get_GE (link)); }
	Lane_Use_Data * Get_LE (int link)      { return ((Lane_Use_Data *) Class_Array::Get_LE (link)); }

	Lane_Use_Data * First (void)           { return ((Lane_Use_Data *) Class_Array::First ()); }
	Lane_Use_Data * Next (void)            { return ((Lane_Use_Data *) Class_Array::Next ()); }
	
	Lane_Use_Data * Last (void)            { return ((Lane_Use_Data *) Class_Array::Last ()); }
	Lane_Use_Data * Previous (void)        { return ((Lane_Use_Data *) Class_Array::Previous ()); }

	Lane_Use_Data * First_Key (void)       { return ((Lane_Use_Data *) Class_Array::First_Key ()); }
	Lane_Use_Data * Next_Key (void)        { return ((Lane_Use_Data *) Class_Array::Next_Key ()); }

	Lane_Use_Data * operator[] (int index) { return (Record (index)); }
};

#endif
