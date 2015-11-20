//*********************************************************
//	Data_Array.hpp - dynamically allocated array
//*********************************************************

#ifndef DATA_ARRAY_HPP
#define DATA_ARRAY_HPP

#include <stdlib.h>

//---------------------------------------------------------
//	Data_Array -- class definition
//---------------------------------------------------------

class Data_Array
{
public:
	Data_Array (int data_size = 0, int max_records = 0);
	virtual ~Data_Array (void)			{ Data_Array::Clear (); }

	bool Initialize (int data_size, int max_records = 0);

	virtual bool Add (void *data = NULL);

	virtual bool Add_Records (int number = 1);

	virtual bool Set (int index, void *data);

	void * New_Record (bool clear = false, int number = 1);

	void * Record (int index)			{ return (Record_Index (index) ? array + index * data_size : NULL); }
	void * Record (void)				{ return (Record (record_index)); }

	void * First (void)					{ return (Record (record_index = 1)); }
	void * Next (void)					{ return (Record (++record_index)); }
	
	void * Last (void)					{ return (Record (record_index = num_records)); }
	void * Previous (void)				{ return (Record (--record_index)); }

	void * Best (int index)				{ return (Record_Index (index) ? Record () : Last ()); }

	void Zero_Fill (void);
	virtual void Reset (void)			{ num_records = 0; };

	int  Data_Size (void)				{ return (data_size); }
	void Data_Size (int size);

	int  Num_Records (void)				{ return (num_records); }
	bool Num_Records (int num_records);

	virtual int  Max_Records (void)		{ return (max_records); }
	virtual bool Max_Records (int max_records);
	virtual void Clear (void);

	int  Record_Index (void)			{ return (record_index); }
	bool Record_Index (int index)		{ return (Check_Index (record_index = index)); }
	int  Record_Index (void *ptr)		{ return ((Record_Index ((int) (((char *) ptr - array) / data_size))) ? record_index : 0); }

	bool Check_Index (int index)		{ return (index > 0 && index <= num_records); }

	void * operator[] (int index)		{ return (Record (index)); }

protected:
	void Flip_Records (void);

private:
	bool Add_Memory (int min_size = 1);

	int data_size;
	int record_index;
	int num_records;
	int max_records;

	char *array;
};
#endif
