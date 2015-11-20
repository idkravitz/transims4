//*********************************************************
//	Link_Data.hpp - network link data
//*********************************************************

#ifndef LINK_DATA_HPP
#define LINK_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Link_Data class definition
//---------------------------------------------------------

class Link_Data : public Class_Index
{
public:
	Link_Data (int link = 0);
	virtual ~Link_Data (void)   {}

	int  Link (void)            { return (ID ()); }
	int  Anode (void)           { return (anode); }
	int  Bnode (void)           { return (bnode); }
	int  Length (void)          { return (length); }
	int  Aoffset (void)         { return (aoffset); }
	int  Boffset (void)         { return (boffset); }
	int  Use (void)             { return (use); }
	int  Type (void)            { return (type); }
	int  AB_Dir (void)          { return (ab_dir); }
	int  BA_Dir (void)          { return (ba_dir); }
	int  Shape (void)           { return (shape); }
	
	void Link (int value)       { ID (value); }
	void Anode (int value)      { anode = value; }
	void Bnode (int value)      { bnode = value; }
	void Length (int value)     { length = value; }
	void Aoffset (int value)    { aoffset = (short) value; }
	void Boffset (int value)    { boffset = (short) value; }
	void Use (int value)        { use = (short) value; }
	void Type (int value)       { type = (short) value; }
	void AB_Dir (int value)     { ab_dir = value; }
	void BA_Dir (int value)     { ba_dir = value; }
	void Shape (int value)      { shape = value; }

private:
	int   anode;
	int   bnode;
	int   length;		//---- rounded ----
	short aoffset;		//---- rounded ----
	short boffset;
	short use;
	short type;
	int   ab_dir;
	int   ba_dir;
	int   shape;
};

//---------------------------------------------------------
//	Link_Array class definition
//---------------------------------------------------------

class Link_Array : public Class_Array
{
public:
	Link_Array (int max_records = 0);

	bool Add (Link_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Link_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Link_Data *) Class_Array::New_Record (clear, number)); }

	Link_Data * Record (int index)          { return ((Link_Data *) Class_Array::Record (index)); }
	Link_Data * Record (void)               { return ((Link_Data *) Class_Array::Record ()); }

	Link_Data * Get (int link)              { return ((Link_Data *) Class_Array::Get (link)); }
	Link_Data * Get_GE (int link)           { return ((Link_Data *) Class_Array::Get_GE (link)); }
	Link_Data * Get_LE (int link)           { return ((Link_Data *) Class_Array::Get_LE (link)); }

	Link_Data * First (void)                { return ((Link_Data *) Class_Array::First ()); }
	Link_Data * Next (void)                 { return ((Link_Data *) Class_Array::Next ()); }
	
	Link_Data * Last (void)                 { return ((Link_Data *) Class_Array::Last ()); }
	Link_Data * Previous (void)             { return ((Link_Data *) Class_Array::Previous ()); }

	Link_Data * First_Key (void)            { return ((Link_Data *) Class_Array::First_Key ()); }
	Link_Data * Next_Key (void)             { return ((Link_Data *) Class_Array::Next_Key ()); }

	Link_Data * operator[] (int index)		{ return (Record (index)); }
};

#endif
