//*********************************************************
//	Link_Detail_Data.hpp - link detail data
//*********************************************************

#ifndef Link_Detail_Data_HPP
#define Link_Detail_Data_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Time_Range.hpp"

//---------------------------------------------------------
//	Link_Detail_Data class definition
//---------------------------------------------------------

class Link_Detail_Data : public Class_Index
{
public:
	Link_Detail_Data (int link_dir = 0);
	virtual ~Link_Detail_Data (void)         {}

	int  Link_Dir (void)                     { return (ID ()); }
	int  Link (void)                         { return ((ID () >> 1)); }
	int  Dir (void)                          { return ((ID () & 1)); }
	int  Type (void)                         { return (type); }
	int  Control (void)                      { return (control); }
	int  Left (void)                         { return (left); }
	int  Left_Thru (void)                    { return (left_thru); }
	int  Thru (void)                         { return (thru); }
	int  Right_Thru (void)                   { return (right_thru); }
	int  Right (void)                        { return (right); }
	int  Use (void)                          { return (use); }
	int  Lanes (void)                        { return (lanes); }
	Time_Range * Period (void)               { return (period); }

	void Link_Dir (int value)                { ID (value); }
	void Link (int value)                    { ID ((value << 1) + Dir ()); }
	void Dir (int value)                     { ID ((Link () << 1) + value); }
	void Type (int value)                    { type = (char) value; }
	void Control (int value)                 { control = (char) value; }
	void Left (int value)                    { left = (short) value; }
	void Left_Thru (int value)               { left_thru = (short) value; }
	void Thru (int value)                    { thru = (short) value; }
	void Right_Thru (int value)              { right_thru = (short) value; }
	void Right (int value)                   { right = (short) value; }
	void Use (int value)                     { use = (short) value; }
	void Lanes (int value)                   { lanes = (short) value; }
	void Period (Time_Range *ptr)            { period = ptr; }

private:
	char  type;
	char  control;
	short left;
	short left_thru;
	short thru;
	short right_thru;
	short right;
	short lanes;
	short use;
	Time_Range *period;
};

//---------------------------------------------------------
//	Link_Detail_Array class definition
//---------------------------------------------------------

class Link_Detail_Array : public Class_Array
{
public:
	Link_Detail_Array (int max_records = 0);
	virtual ~Link_Detail_Array (void)              { Clear (); }

	bool Add (Link_Detail_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Link_Detail_Data * New_Record (bool clear = false, int number = 1)
	                                               { return ((Link_Detail_Data *) Class_Array::New_Record (clear, number)); }

	Link_Detail_Data * Record (int index)          { return ((Link_Detail_Data *) Class_Array::Record (index)); }
	Link_Detail_Data * Record (void)               { return ((Link_Detail_Data *) Class_Array::Record ()); }

	Link_Detail_Data * Get (int link)              { return ((Link_Detail_Data *) Class_Array::Get (link)); }
	Link_Detail_Data * Get_GE (int link)           { return ((Link_Detail_Data *) Class_Array::Get_GE (link)); }
	Link_Detail_Data * Get_LE (int link)           { return ((Link_Detail_Data *) Class_Array::Get_LE (link)); }

	Link_Detail_Data * First (void)                { return ((Link_Detail_Data *) Class_Array::First ()); }
	Link_Detail_Data * Next (void)                 { return ((Link_Detail_Data *) Class_Array::Next ()); }
	
	Link_Detail_Data * Last (void)                 { return ((Link_Detail_Data *) Class_Array::Last ()); }
	Link_Detail_Data * Previous (void)             { return ((Link_Detail_Data *) Class_Array::Previous ()); }

	Link_Detail_Data * First_Key (void)            { return ((Link_Detail_Data *) Class_Array::First_Key ()); }
	Link_Detail_Data * Next_Key (void)             { return ((Link_Detail_Data *) Class_Array::Next_Key ()); }

	Link_Detail_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
