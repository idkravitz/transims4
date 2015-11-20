//*********************************************************
//	Dir_Data.hpp - network link direction data
//*********************************************************

#ifndef DIR_DATA_HPP
#define DIR_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Dir_Data class definition
//---------------------------------------------------------

class Dir_Data : public Class_Index
{
public:
	Dir_Data (int link_dir = 0);
	virtual ~Dir_Data (void)                 {}

	int  Link_Dir (void)                     { return (ID ()); }
	int  Link (void)                         { return ((ID () >> 1)); }
	int  Dir (void)                          { return ((ID () & 1)); }
	int  Thru (void)                         { return (thru); }
	int  Left (void)                         { return (left); }
	int  Right (void)                        { return (right); }
	int  Speed (void)                        { return (speed); }
	int  Capacity (void)                     { return (capacity); }
	int  In_Bearing (void)                   { return (in_bear); }
	int  Out_Bearing (void)                  { return (out_bear); }
	int  Cost_List (void)                    { return (cost_list); }
	int  TOD_List (void)                     { return (tod_list); }
	int  Time0 (void)                        { return (time0); }	

	void Link_Dir (int value)                { ID (value); }
	void Link (int value)                    { ID ((value << 1) + Dir ()); }
	void Dir (int value)                     { ID ((Link () << 1) + value); }
	void Thru (int value)                    { thru = (short) value; }
	void Left (int value)                    { left = (unsigned char) value; }
	void Right (int value)                   { right = (unsigned char) value; }
	void Speed (int value)                   { speed = (unsigned short) value; }
	void Capacity (int value)                { capacity = (unsigned short) value; }
	void In_Bearing (int value)              { in_bear = (short) value; }
	void Out_Bearing (int value)             { out_bear = (short) value; }
	void Cost_List (int value)               { cost_list = value; }
	void TOD_List (int value)                { tod_list = value; }
	void Time0 (int value)                   { time0 = value; }

	//---- overloaded methods ----

	int  Stop_List (void)                    { return (tod_list); }
	void Stop_List (int value)               { tod_list = value; }

private:
	unsigned short thru;
	unsigned char  left;
	unsigned char  right;
	unsigned short speed;		//---- rounded ----
	unsigned short capacity;
	short          in_bear;
	short          out_bear;
	int            cost_list;
	int            tod_list;
	int            time0;		//---- rounded ----
};

//---------------------------------------------------------
//	Dir_Array class definition
//---------------------------------------------------------

class Dir_Array : public Class_Array
{
public:
	Dir_Array (int max_records = 0);
	virtual ~Dir_Array (void)              { Clear (); }

	bool Add (Dir_Data *data = NULL)       { return (Class_Array::Add (data)); }
	
	Dir_Data * New_Record (bool clear = false, int number = 1)
	                                       { return ((Dir_Data *) Class_Array::New_Record (clear, number)); }

	Dir_Data * Record (int index)          { return ((Dir_Data *) Class_Array::Record (index)); }
	Dir_Data * Record (void)               { return ((Dir_Data *) Class_Array::Record ()); }

	Dir_Data * Get (int link)              { return ((Dir_Data *) Class_Array::Get (link)); }
	Dir_Data * Get_GE (int link)           { return ((Dir_Data *) Class_Array::Get_GE (link)); }
	Dir_Data * Get_LE (int link)           { return ((Dir_Data *) Class_Array::Get_LE (link)); }

	Dir_Data * First (void)                { return ((Dir_Data *) Class_Array::First ()); }
	Dir_Data * Next (void)                 { return ((Dir_Data *) Class_Array::Next ()); }
	
	Dir_Data * Last (void)                 { return ((Dir_Data *) Class_Array::Last ()); }
	Dir_Data * Previous (void)             { return ((Dir_Data *) Class_Array::Previous ()); }

	Dir_Data * First_Key (void)            { return ((Dir_Data *) Class_Array::First_Key ()); }
	Dir_Data * Next_Key (void)             { return ((Dir_Data *) Class_Array::Next_Key ()); }

	Dir_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
