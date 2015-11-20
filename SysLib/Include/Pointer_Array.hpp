//*********************************************************
//	Pointer_Array.hpp - array of indexed pointers
//*********************************************************

#ifndef POINTER_ARRAY_HPP
#define POINTER_ARRAY_HPP

#include "Data_Array.hpp"
#include "Index_Array.hpp"

//---------------------------------------------------------
//	Pointer_Array class definition
//---------------------------------------------------------

class Pointer_Array
{
public:
	Pointer_Array (void);
	virtual ~Pointer_Array (void);

	virtual void Delete_Pointers (void)	{}

protected:
	void * Pointer (void *ptr)      { return ((void *) ((ptr != NULL) ? *((void **) ptr) : ptr)); }

	void **data_ptr;
};

//---------------------------------------------------------
//	Data_Pointer_Array class definition
//---------------------------------------------------------

class Data_Pointer_Array : public Data_Array, public Pointer_Array
{
public:
	Data_Pointer_Array (int max_records = 0);
	virtual ~Data_Pointer_Array (void)   { Delete_Pointers (); Clear (); }

	virtual void Delete_Pointers (void);

	virtual bool Add (void *data)   { *data_ptr = data; return (Data_Array::Add (data_ptr)); }

	void * Record (int index)       { return (Pointer (Data_Array::Record (index))); }
	void * Record (void)            { return (Pointer (Data_Array::Record ())); }

	void * First (void)             { return (Pointer (Data_Array::First ())); }
	void * Next (void)              { return (Pointer (Data_Array::Next ())); }
	
	void * Last (void)              { return (Pointer (Data_Array::Last ())); }
	void * Previous (void)          { return (Pointer (Data_Array::Previous ())); }

	void * Best (int index)         { return (Pointer (Data_Array::Best (index))); }

	void * operator[] (int index)   { return (Record (index)); }
};

//---------------------------------------------------------
//	Index_Pointer_Array class definition
//---------------------------------------------------------

class Index_Pointer_Array : public Index_Array, public Pointer_Array
{
public:
	Index_Pointer_Array (int max_records = 0, bool sorted = false);
	virtual ~Index_Pointer_Array (void)   { Delete_Pointers (); Clear (); }

	virtual int Sort_Function (void *data1, void *data2);
	virtual bool Split_Function (void *data1, void *data2);

	virtual void Delete_Pointers (void);

	virtual bool Add (void *data)   { *data_ptr = data; return (Index_Array::Add (data_ptr)); }

	int Get_Index (void *data)      { *data_ptr = data; return (Index_Array::Get_Index (data_ptr)); }

	void * Get (void *data)         { *data_ptr = data; return (Pointer (Index_Array::Get (data_ptr))); }
	void * Get_GE (void *data)      { *data_ptr = data; return (Pointer (Index_Array::Get_GE (data_ptr))); }
	void * Get_LE (void *data)      { *data_ptr = data; return (Pointer (Index_Array::Get_LE (data_ptr))); }
	
	void * First_Key (void)         { return (Pointer (Index_Array::First_Key ())); }
	void * Next_Key (void)          { return (Pointer (Index_Array::Next_Key ())); }

	void * Last_Key (void)          { return (Pointer (Index_Array::Last_Key ())); }
	void * Previous_Key (void)      { return (Pointer (Index_Array::Previous_Key ())); }

	void * Record (int index)       { return (Pointer (Index_Array::Record (index))); }
	void * Record (void)            { return (Pointer (Index_Array::Record ())); }

	void * First (void)             { return (Pointer (Index_Array::First ())); }
	void * Next (void)              { return (Pointer (Index_Array::Next ())); }
	
	void * Last (void)              { return (Pointer (Index_Array::Last ())); }
	void * Previous (void)          { return (Pointer (Index_Array::Previous ())); }

	void * Best (int index)         { return (Pointer (Index_Array::Best (index))); }

	void * operator[] (int index)   { return (Record (index)); }
};

#endif
