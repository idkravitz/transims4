//*********************************************************
//	HHList_Data.hpp - household partition data
//*********************************************************

#ifndef HHLIST_DATA_HPP
#define HHLIST_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	HHList_Data class definition
//---------------------------------------------------------

class HHList_Data : public Class_Index
{
public:
	HHList_Data (int household = 0);

	int    Household (void)            { return (ID ()); }
	int    File (void)                 { return (file); }
	bool   Keep (void)                 { return (keep); }

	void   Household (int value)       { ID (value); }
	void   File (int value)            { file = value; }
	void   Keep (bool value)           { keep = value; }

private:
	int file;
	bool keep;
};

//---------------------------------------------------------
//	HHList_Array class definition
//---------------------------------------------------------

class HHList_Array : public Class_Array
{
public:
	HHList_Array (int max_records = 0);

	bool Add (HHList_Data *data = NULL)     { return (Class_Array::Add (data)); }
	
	HHList_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((HHList_Data *) Class_Array::New_Record (clear, number)); }

	HHList_Data * Get (int id)              { return ((HHList_Data *) Class_Array::Get (id)); }

	HHList_Data * Record (int index)        { return ((HHList_Data *) Class_Array::Record (index)); }
	HHList_Data * Record (void)             { return ((HHList_Data *) Class_Array::Record ()); }

	HHList_Data * First (void)              { return ((HHList_Data *) Class_Array::First ()); }
	HHList_Data * Next (void)               { return ((HHList_Data *) Class_Array::Next ()); }
	
	HHList_Data * First_Key (void)          { return ((HHList_Data *) Class_Array::First_Key ()); }
	HHList_Data * Next_Key (void)           { return ((HHList_Data *) Class_Array::Next_Key ()); }

	HHList_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
