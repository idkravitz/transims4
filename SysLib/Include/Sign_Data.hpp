//*********************************************************
//	Sign_Data.hpp - network unsignalized node data
//*********************************************************

#ifndef SIGN_DATA_HPP
#define SIGN_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Sign_Data class definition
//---------------------------------------------------------

class Sign_Data : public Class_Index
{
public:
	Sign_Data (int link_dir = 0);
	virtual ~Sign_Data (void) {}

	int  Link_Dir (void)      { return (ID ()); }
	int  Link (void)          { return ((ID () >> 1)); }
	int  Dir (void)           { return ((ID () & 1)); }
	int  Sign (void)          { return (sign); }

	void Link_Dir (int value) { ID (value); }
	void Link (int value)     { ID ((value << 1) + Dir ()); }
	void Dir (int value)      { ID ((Link () << 1) + value); }
	void Sign (int value)     { sign = value; }

private:
	int  sign;
};

//---------------------------------------------------------
//	Sign_Array class definition
//---------------------------------------------------------

class Sign_Array : public Class_Array
{
public:
	Sign_Array (int max_records = 0);

	bool Add (Sign_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Sign_Data * New_Record (bool clear = false, int number = 1)
	                                     { return ((Sign_Data *) Class_Array::New_Record (clear, number)); }

	Sign_Data * Record (int index)       { return ((Sign_Data *) Class_Array::Record (index)); }
	Sign_Data * Record (void)            { return ((Sign_Data *) Class_Array::Record ()); }

	Sign_Data * Get (int link_dir)       { return ((Sign_Data *) Class_Array::Get (link_dir)); }
	Sign_Data * Get_GE (int link_dir)    { return ((Sign_Data *) Class_Array::Get_GE (link_dir)); }
	Sign_Data * Get_LE (int link_dir)    { return ((Sign_Data *) Class_Array::Get_LE (link_dir)); }

	Sign_Data * First (void)             { return ((Sign_Data *) Class_Array::First ()); }
	Sign_Data * Next (void)              { return ((Sign_Data *) Class_Array::Next ()); }
	
	Sign_Data * Last (void)              { return ((Sign_Data *) Class_Array::Last ()); }
	Sign_Data * Previous (void)          { return ((Sign_Data *) Class_Array::Previous ()); }

	Sign_Data * First_Key (void)         { return ((Sign_Data *) Class_Array::First_Key ()); }
	Sign_Data * Next_Key (void)          { return ((Sign_Data *) Class_Array::Next_Key ()); }

	Sign_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
