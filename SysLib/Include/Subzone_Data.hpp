//*********************************************************
//	Subzone_Data.hpp - subzone data
//*********************************************************

#ifndef SUBZONE_DATA_HPP
#define SUBZONE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Subzone_Data class definition
//---------------------------------------------------------

class Subzone_Data : public Class2_Index
{
public:
	Subzone_Data (int zone = 0, int id = 0);
	virtual ~Subzone_Data (void)  {}

	int  ID (void)             { return (Key2 ()); }
	int  Zone (void)           { return (Key1 ()); }
	int  X (void)              { return (x); }
	int  Y (void)              { return (y); }
	int  Data (void)           { return (data); }

	void ID (int value)        { Key2 (value); }
	void Zone (int value)      { Key1 (value); }
	void X (int value)         { x = value; }
	void Y (int value)         { y = value; }
	void Data (int value)      { data = value; }

private:
	int  x;				//---- rounded ----
	int  y;				//---- rounded ----
	int  data;
};

//---------------------------------------------------------
//	Subzone_Array class definition
//---------------------------------------------------------

class Subzone_Array : public Class2_Array
{
public:
	Subzone_Array (int max_records = 0);

	bool Add (Subzone_Data *data = NULL)    { return (Class2_Array::Add (data)); }
	
	Subzone_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Subzone_Data *) Class2_Array::New_Record (clear, number)); }

	Subzone_Data * Record (int index)       { return ((Subzone_Data *) Class2_Array::Record (index)); }
	Subzone_Data * Record (void)            { return ((Subzone_Data *) Class2_Array::Record ()); }
	
	Subzone_Data * Get (int zone, int id)   { return ((Subzone_Data *) Class2_Array::Get (zone, id)); }
	Subzone_Data * Get_GE (int zone)        { return ((Subzone_Data *) Class2_Array::Get_GE (zone, 0)); }

	Subzone_Data * First (void)             { return ((Subzone_Data *) Class2_Array::First ()); }
	Subzone_Data * Next (void)              { return ((Subzone_Data *) Class2_Array::Next ()); }
	
	Subzone_Data * Last (void)              { return ((Subzone_Data *) Class2_Array::Last ()); }
	Subzone_Data * Previous (void)          { return ((Subzone_Data *) Class2_Array::Previous ()); }

	Subzone_Data * First_Key (void)         { return ((Subzone_Data *) Class2_Array::First_Key ()); }
	Subzone_Data * Next_Key (void)          { return ((Subzone_Data *) Class2_Array::Next_Key ()); }

	Subzone_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
