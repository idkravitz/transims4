//*********************************************************
//	Class_Array.hpp - class data arrays
//*********************************************************

#ifndef CLASS_ARRAY_HPP
#define CLASS_ARRAY_HPP

#include "Index_Array.hpp"
#include "Complex_Array.hpp"
#include "Class_Index.hpp"

//---------------------------------------------------------
//	Class_Array class definition
//---------------------------------------------------------

class Class_Array : public Index_Array
{
public:
	Class_Array (int data_size = sizeof (Class_Index), int max_records = 0, bool sorted = false);

	virtual int Sort_Function (void *data1, void *data2);
	virtual bool Split_Function (void *data1, void *data2);

	bool Add (Class_Index *data = NULL)      { return (Index_Array::Add (data)); }
	
	bool Set (int index, Class_Index *data)  { return (Index_Array::Set (index, data)); }

	int Get_Index (int id)                   { Class_Index key (id); return (Get_Index (&key)); }
	int Get_Index (Class_Index *data)        { return (Index_Array::Get_Index (data)); }	

	Class_Index * Record (int index)         { return ((Class_Index *) Index_Array::Record (index)); }
	Class_Index * Record (void)              { return ((Class_Index *) Index_Array::Record ()); }

	Class_Index * Get (int id)               { Class_Index key (id); return (Get (&key));  }
	Class_Index * Get (Class_Index *data)    { return ((Class_Index *) Index_Array::Get (data)); }

	Class_Index * Get_GE (int id)            { Class_Index key (id); return (Get_GE (&key)); }
	Class_Index * Get_GE (Class_Index *data) { return ((Class_Index *) Index_Array::Get_GE (data)); }

	Class_Index * Get_LE (int id)            { Class_Index key (id); return (Get_LE (&key)); }
	Class_Index * Get_LE (Class_Index *data) { return ((Class_Index *) Index_Array::Get_LE (data)); }

	Class_Index * First (void)               { return ((Class_Index *) Index_Array::First ()); }
	Class_Index * Next (void)                { return ((Class_Index *) Index_Array::Next ()); }
	
	Class_Index * Last (void)                { return ((Class_Index *) Index_Array::Last ()); }
	Class_Index * Previous (void)            { return ((Class_Index *) Index_Array::Previous ()); }

	Class_Index * First_Key (void)           { return ((Class_Index *) Index_Array::First_Key ()); }
	Class_Index * Next_Key (void)            { return ((Class_Index *) Index_Array::Next_Key ()); }

	Class_Index * Last_Key (void)            { return ((Class_Index *) Index_Array::Last_Key ()); }
	Class_Index * Previous_Key (void)        { return ((Class_Index *) Index_Array::Previous_Key ()); }
};

//---------------------------------------------------------
//	Class2_Array class definition
//---------------------------------------------------------

class Class2_Array : public Complex_Array
{
public:
	Class2_Array (int data_size = sizeof (Class2_Index), int max_records = 0, bool sorted = false);

	virtual int Sort_Function (void *data1, void *data2);
	virtual bool Split_Function (void *data1, void *data2);

	bool Add (Class2_Index *data = NULL)       { return (Complex_Array::Add (data)); }
	
	bool Set (int index, Class2_Index *data)   { return (Complex_Array::Set (index, data)); }

	int Get_Index (int key1, int key2)         { Class2_Index key (key1, key2); return (Get_Index (&key)); }
	int Get_Index (Class2_Index *data)         { return (Complex_Array::Get_Index (data)); }	

	Class2_Index * Record (int index)          { return ((Class2_Index *) Complex_Array::Record (index)); }
	Class2_Index * Record (void)               { return ((Class2_Index *) Complex_Array::Record ()); }

	Class2_Index * Get (int key1, int key2)    { Class2_Index key (key1, key2); return (Get (&key)); }
	Class2_Index * Get (Class2_Index *data)    { return ((Class2_Index *) Complex_Array::Get (data)); }

	Class2_Index * Get_GE (int key1, int key2) { Class2_Index key (key1, key2); return (Get_GE (&key)); }
	Class2_Index * Get_GE (Class2_Index *data) { return ((Class2_Index *) Complex_Array::Get_GE (data)); }

	Class2_Index * Get_LE (int key1, int key2) { Class2_Index key (key1, key2); return (Get_LE (&key)); }
	Class2_Index * Get_LE (Class2_Index *data) { return ((Class2_Index *) Complex_Array::Get_LE (data)); }

	Class2_Index * First (void)	               { return ((Class2_Index *) Complex_Array::First ()); }
	Class2_Index * Next (void)                 { return ((Class2_Index *) Complex_Array::Next ()); }
	
	Class2_Index * Last (void)                 { return ((Class2_Index *) Complex_Array::Last ()); }
	Class2_Index * Previous (void)             { return ((Class2_Index *) Complex_Array::Previous ()); }

	Class2_Index * First_Key (void)            { return ((Class2_Index *) Complex_Array::First_Key ()); }
	Class2_Index * Next_Key (void)             { return ((Class2_Index *) Complex_Array::Next_Key ()); }

	Class2_Index * Last_Key (void)             { return ((Class2_Index *) Complex_Array::Last_Key ()); }
	Class2_Index * Previous_Key (void)         { return ((Class2_Index *) Complex_Array::Previous_Key ()); }
};

#endif
