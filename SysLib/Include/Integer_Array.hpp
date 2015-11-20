//*********************************************************
//	Integer_Array.hpp - dynamic integer array
//*********************************************************

#ifndef INTEGER_ARRAY_HPP
#define INTEGER_ARRAY_HPP

#include "Index_Array.hpp"

//---------------------------------------------------------
//	Integer_List
//---------------------------------------------------------

class Integer_List : public Data_Array
{
public:
	Integer_List (int max_records = 0);
	
	bool Add (int data)             { return (Add (&data)); }
	bool Add (int *data = NULL)		{ return (Data_Array::Add (data)); }

	bool Set (int index, int data)  { return (Data_Array::Set (index, &data)); }
	
	int * Record (int index)        { return ((int *) Data_Array::Record (index)); }
	int * Record (void)             { return ((int *) Data_Array::Record ()); }

	int First (void)                { return (Check_Pointer (Data_Array::First ())); }
	int Next (void)                 { return (Check_Pointer (Data_Array::Next ())); }
	
	int Last (void)                 { return (Check_Pointer (Data_Array::Last ())); }
	int Previous (void)             { return (Check_Pointer (Data_Array::Previous ())); }

	int Best (int index)			{ return (Check_Pointer (Data_Array::Best (index))); }

	int Value (int index)			{ return (Check_Pointer (Record (index))); }
	int operator[] (int index)      { return (Check_Pointer (Record (index))); }

	int Check_Pointer (void *ptr)	{ return ((ptr) ? *((int *) ptr) : 0); }

	void Flip_List (void)           { Data_Array::Flip_Records (); }
};

//---------------------------------------------------------
//	Integer_Sort
//---------------------------------------------------------

class Integer_Sort : public Index_Array
{
public:
	Integer_Sort (int max_records = 0, bool sorted = false);

	bool Add (int data)             { return (Add (&data)); }
	bool Add (int *data = NULL)     { return (Index_Array::Add (data)); }
	
	bool Set (int index, int data)  { return (Index_Array::Set (index, &data)); }

	int * Record (int index)        { return ((int *) Index_Array::Record (index)); }
	int * Record (void)             { return ((int *) Index_Array::Record ()); }

	int Get_Index (int data)        { return (Index_Array::Get_Index (&data)); }
	
	int First_Key (void)            { return (Check_Pointer (Index_Array::First_Key ())); }
	int Next_Key (void)             { return (Check_Pointer (Index_Array::Next_Key ())); }
	
	int Last_Key (void)             { return (Check_Pointer (Index_Array::Last_Key ())); }
	int Previous_Key (void)         { return (Check_Pointer (Index_Array::Previous_Key ())); }

	int First (void)                { return (Check_Pointer (Index_Array::First ())); }
	int Next (void)                 { return (Check_Pointer (Index_Array::Next ())); }
	
	int Last (void)                 { return (Check_Pointer (Index_Array::Last ())); }
	int Previous (void)             { return (Check_Pointer (Index_Array::Previous ())); }

	int operator[] (int index)      { return (Check_Pointer (Record (index))); }

	int Check_Pointer (void *ptr)	{ return ((ptr) ? *((int *) ptr) : 0); }
};

#endif
