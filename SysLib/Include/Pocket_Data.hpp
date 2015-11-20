//*********************************************************
//	Pocket_Data.hpp - network pocket lane data
//*********************************************************

#ifndef POCKET_DATA_HPP
#define POCKET_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Pocket_Data class definition
//---------------------------------------------------------

class Pocket_Data : public Class_Index
{
public:
	Pocket_Data (int id = 0);
	virtual ~Pocket_Data (void)  {}

	int  Link_Dir (void)         { return (link_dir); }
	int  Link (void)             { return ((link_dir >> 1)); }
	int  Dir (void)              { return ((link_dir & 1)); }
	int  Offset (void)           { return (offset); }
	int  Lane (void)             { return (lane); }
	int  Type (void)             { return (type); }
	int  Length (void)           { return (length); } 

	void Link_Dir (int value)    { link_dir = value; }
	void Link (int value)        { link_dir = (value << 1) + Dir (); }
	void Dir (int value)         { link_dir = (Link () << 1) + value; }
	void Offset (int value)      { offset = value; }
	void Lane (int value)        { lane = (unsigned char) value; }
	void Type (int value)        { type = (unsigned char) value; }
	void Length (int value)      { length = (unsigned short) value; }

private:
	int            link_dir;
	int            offset;		//---- rounded ----
	unsigned char  lane;
	unsigned char  type;
	unsigned short length;		//---- rounded ----
};

//---------------------------------------------------------
//	Pocket_Array class definition
//---------------------------------------------------------

class Pocket_Array : public Class_Array
{
public:
	Pocket_Array (int max_records = 0);

	bool Add (Pocket_Data *data = NULL)   { return (Class_Array::Add (data)); }
	
	Pocket_Data * New_Record (bool clear = false, int number = 1)
	                                      { return ((Pocket_Data *) Class_Array::New_Record (clear, number)); }

	Pocket_Data * Record (int index)      { return ((Pocket_Data *) Class_Array::Record (index)); }
	Pocket_Data * Record (void)           { return ((Pocket_Data *) Class_Array::Record ()); }

	Pocket_Data * Get (int id)            { return ((Pocket_Data *) Class_Array::Get (id)); }
	Pocket_Data * Get_GE (int id)         { return ((Pocket_Data *) Class_Array::Get_GE (id)); }
	Pocket_Data * Get_LE (int id)         { return ((Pocket_Data *) Class_Array::Get_LE (id)); }

	Pocket_Data * First (void)            { return ((Pocket_Data *) Class_Array::First ()); }
	Pocket_Data * Next (void)             { return ((Pocket_Data *) Class_Array::Next ()); }
	
	Pocket_Data * Last (void)             { return ((Pocket_Data *) Class_Array::Last ()); }
	Pocket_Data * Previous (void)         { return ((Pocket_Data *) Class_Array::Previous ()); }

	Pocket_Data * First_Key (void)        { return ((Pocket_Data *) Class_Array::First_Key ()); }
	Pocket_Data * Next_Key (void)         { return ((Pocket_Data *) Class_Array::Next_Key ()); }

	Pocket_Data * operator[] (int index)  { return (Record (index)); }
};

#endif
