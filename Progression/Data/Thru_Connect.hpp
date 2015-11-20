//*********************************************************
//	Thru_Connect.hpp - thru connections to upstream link
//*********************************************************

#ifndef THRU_CONNECT_HPP
#define THRU_CONNECT_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Thru_Connect class definition
//---------------------------------------------------------

class Thru_Connect : public Class_Index
{
public:
	Thru_Connect (int link_dir = 0);
	virtual ~Thru_Connect (void)     {}

	int  Link_Dir (void)            { return (ID ()); }
	int  Link (void)                { return ((ID () >> 1)); }
	int  Dir (void)                 { return ((ID () & 1)); }
	int  Thru_Link_Dir (void)       { return (thru_link_dir); }
	int  Thru_Link (void)           { return (thru_link_dir >> 1); }
	int  Thru_Dir (void)            { return (thru_link_dir & 1); } 

	void Link_Dir (int value)       { ID (value); }
	void Link (int value)           { ID ((value << 1) + Dir ()); }
	void Dir (int value)            { ID ((Link () << 1) + value); }
	void Thru_Link_Dir (int value)  { thru_link_dir = value; }
	void Thru_Link (int value)      { thru_link_dir = ((value << 1) + Thru_Dir ()); }
	void Thru_Dir (int value)       { thru_link_dir = ((Thru_Link () << 1) + value); }

private:
	int thru_link_dir;
};

//---------------------------------------------------------
//	Thru_Connect_Array class definition
//---------------------------------------------------------

class Thru_Connect_Array : public Class_Array
{
public:
	Thru_Connect_Array (int max_records = 0);

	bool Add (Thru_Connect *data = NULL)     { return (Class_Array::Add (data)); }
	
	Thru_Connect * New_Record (bool clear = false, int number = 1)
	                                        { return ((Thru_Connect *) Class_Array::New_Record (clear, number)); }

	Thru_Connect * Record (int index)        { return ((Thru_Connect *) Class_Array::Record (index)); }
	Thru_Connect * Record (void)             { return ((Thru_Connect *) Class_Array::Record ()); }

	Thru_Connect * Get (int link_dir)        { return ((Thru_Connect *) Class_Array::Get (link_dir)); }
	Thru_Connect * Get (Thru_Connect *data)  { return ((Thru_Connect *) Class_Array::Get (data)); }

	Thru_Connect * First (void)              { return ((Thru_Connect *) Class_Array::First ()); }
	Thru_Connect * Next (void)               { return ((Thru_Connect *) Class_Array::Next ()); }

	Thru_Connect * First_Key (void)          { return ((Thru_Connect *) Class_Array::First_Key ()); }
	Thru_Connect * Next_Key (void)           { return ((Thru_Connect *) Class_Array::Next_Key ()); }

	Thru_Connect * operator[] (int index)    { return (Record (index)); }
};

#endif
