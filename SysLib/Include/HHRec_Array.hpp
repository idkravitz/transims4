//*********************************************************
//	HHRec_Array.hpp - household record index array
//*********************************************************

#ifndef HHREC_ARRAY_HPP
#define HHREC_ARRAY_HPP

#include "Index_Array.hpp"

//---------------------------------------------------------
//	HHRec_Index class definition
//---------------------------------------------------------

class HHRec_Index
{
public:
	HHRec_Index (int household = 0, int person = 0, int record = 0) 
		{ Household (household); Person (person); Record (record); }
	virtual ~HHRec_Index (void) {}

	int  Household (void)          { return (household); }
	int  Person (void)             { return (person); }
	int  Record (void)             { return (record); }

	void Household (int value)     { household = value; }
	void Person (int value)        { person = (unsigned short) value; }
	void Record (int value)        { record = (unsigned short) value; }

private:
	int            household;	//---- key1 ----
	unsigned short person;		//---- key2 ----
	unsigned short record;		//---- key3 ----
};

//---------------------------------------------------------
//	HHRec_Array class definition
//---------------------------------------------------------

class HHRec_Array : public Index_Array
{
public:
	HHRec_Array (int data_size = sizeof (HHRec_Index), int max_records = 0, bool sorted = false);

	bool Add (HHRec_Index *data = NULL)            { return (Index_Array::Add (data)); }
	
	HHRec_Index * New_Record (bool clear = false, int number = 1)
 	                                               { return ((HHRec_Index *) Index_Array::New_Record (clear, number)); }

	int Get_Index (int h, int p=0, int r=0)        { HHRec_Index key (h, p, r); return (Get_Index (&key)); }
	int Get_Index (HHRec_Index *data)              { return (Index_Array::Get_Index (data)); }	

	HHRec_Index * Record (int index)               { return ((HHRec_Index *) Index_Array::Record (index)); }
	HHRec_Index * Record (void)                    { return ((HHRec_Index *) Index_Array::Record ()); }

	HHRec_Index * Get (int h, int p=0, int r=0)    { HHRec_Index key (h, p, r); return (Get (&key));  }
	HHRec_Index * Get (HHRec_Index *data)          { return ((HHRec_Index *) Index_Array::Get (data)); }

	HHRec_Index * Get_GE (int h, int p=0, int r=0) { HHRec_Index key (h, p, r); return (Get_GE (&key)); }
	HHRec_Index * Get_GE (HHRec_Index *data)       { return ((HHRec_Index *) Index_Array::Get_GE (data)); }

	HHRec_Index * Get_LE (int h, int p=0, int r=0) { HHRec_Index key (h, p, r); return (Get_LE (&key)); }
	HHRec_Index * Get_LE (HHRec_Index *data)       { return ((HHRec_Index *) Index_Array::Get_LE (data)); }

	HHRec_Index * First (void)                     { return ((HHRec_Index *) Index_Array::First ()); }
	HHRec_Index * Next (void)                      { return ((HHRec_Index *) Index_Array::Next ()); }
	
	HHRec_Index * Last (void)                      { return ((HHRec_Index *) Index_Array::Last ()); }
	HHRec_Index * Previous (void)                  { return ((HHRec_Index *) Index_Array::Previous ()); }

	HHRec_Index * First_Key (void)                 { return ((HHRec_Index *) Index_Array::First_Key ()); }
	HHRec_Index * Next_Key (void)                  { return ((HHRec_Index *) Index_Array::Next_Key ()); }

	HHRec_Index * Last_Key (void)                  { return ((HHRec_Index *) Index_Array::Last_Key ()); }
	HHRec_Index * Previous_Key (void)              { return ((HHRec_Index *) Index_Array::Previous_Key ()); }

	HHRec_Index * operator[] (int index)           { return (Record (index)); }

private:
	virtual int  Sort_Function (void *data1, void *data2); 
	virtual bool Split_Function (void *data1, void *data2);
};

#endif
