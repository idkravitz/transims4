//*********************************************************
//	Data_Array.cpp - dynamically allocated array
//*********************************************************

#include "Data_Array.hpp"
#include <stdio.h>
#include <string.h>

//---------------------------------------------------------
//	Data_Array constructor
//---------------------------------------------------------

Data_Array::Data_Array (int _data_size, int _max_records)
{
	array = NULL;
	data_size = num_records = max_records = record_index = 0;

	Initialize (_data_size, _max_records);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Data_Array::Initialize (int _data_size, int _max_records)
{
	Data_Size (_data_size);
	return (Max_Records (_max_records));
}

//---------------------------------------------------------
//	Add
//---------------------------------------------------------

bool Data_Array::Add (void *data)
{
	if (data_size <= 0) return (false);
	
	if (num_records >= max_records) {
		if (!Add_Memory ()) return (false);
	}
	record_index = ++num_records;

	if (data != NULL) {
		char *record = array + record_index * data_size;

		if (record != (char *) data) {
			memcpy (record, data, data_size);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Records
//---------------------------------------------------------

bool Data_Array::Add_Records (int number)
{
	if (data_size <= 0) return (false);
	
	if (num_records + number >= max_records) return (false);

	record_index = num_records += number;

	return (true);
}

//---------------------------------------------------------
//	Set
//---------------------------------------------------------

bool Data_Array::Set (int index, void *data)
{
	if (data_size <= 0 || !Check_Index (index) || data == NULL) return (false);

	char *record = array + index * data_size;

	if (record != (char *) data) {
		memcpy (record, data, data_size);
	}
	return (true);
}
	
//---------------------------------------------------------
//	New_Record
//---------------------------------------------------------

void * Data_Array::New_Record (bool clear, int number)
{
	if (data_size <= 0) return (NULL);

	int index = num_records + 1;

	if (index + number - 1 > max_records) {	
		if (!Add_Memory (num_records + number - max_records)) return (NULL);
	}
	char *record = array + index * data_size;
	
	if (clear) memset (record, '\0', number * data_size);

	return ((void *) record);
}

//---------------------------------------------------------
//	Data_Size
//---------------------------------------------------------

void Data_Array::Data_Size (int size)		
{
	data_size = size; 
	num_records = 0;

	if (max_records > 0) {
		Max_Records (max_records);
	}
}

//---------------------------------------------------------
//	Zero_Fill
//---------------------------------------------------------

void Data_Array::Zero_Fill (void)
{
	if (array != NULL && num_records > 0) {
		memset (array, '\0', (num_records + 1) * data_size);
	}
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Data_Array::Clear (void)
{
	if (array != NULL) {
		free (array); 
		array = NULL; 
	}
	num_records = max_records = 0;
}

//---------------------------------------------------------
//	Num_Records
//---------------------------------------------------------

bool Data_Array::Num_Records (int max_size)
{
	if (Max_Records (max_size)) {
		if (num_records == 0) {
			num_records = max_size;
		} else {
			num_records = max_size;
		}
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Max_Records
//---------------------------------------------------------

bool Data_Array::Max_Records (int max_size)
{
	if (max_size == 0) {
		Clear ();
		return (true);
	}
	if (max_size < 0 || data_size <= 0) return (false);

	int num = num_records;
	num_records = max_records = 0;

	size_t size = (size_t) (max_size + 1) * data_size;

	array = (char *) realloc (array, size);

	if (array == NULL) return (false);

	char *start = array;
	
	if (num > 0) {
		if (max_size > num) {
			start += (num + 1) * data_size;
			size = (size_t) (max_size - num) * data_size;
		} else {
			size = 0;
		}
	}
	if (size > 0) memset (start, '\0', size);

	max_records = max_size;

	if (num > max_records) {
		num_records = max_records;
	} else {
		num_records = num;
	}
	return (true);
}

//---------------------------------------------------------
//	Flip_Records
//---------------------------------------------------------

void Data_Array::Flip_Records (void)
{
	if (data_size <= 0 || array == NULL || num_records <= 1) return;

	int i, num;
	char *rec1, *rec2;

	num = num_records / 2;

	rec1 = array + data_size;
	rec2 = array + num_records * data_size;

	for (i=1; i <= num; i++, rec1 += data_size, rec2 -= data_size) {
		memcpy (array, rec1, data_size);
		memcpy (rec1, rec2, data_size);
		memcpy (rec2, array, data_size);
	}
}
	
//---------------------------------------------------------
//	Add_Memory
//---------------------------------------------------------

bool Data_Array::Add_Memory (int min_size)
{
	if (data_size <= 0) return (false);

	int size = (1 << 16) / data_size;
	
	if (max_records > 0 && max_records < size) {
		size = max_records >> 1;
	}
	if (max_records + size < min_size) size = min_size - max_records;
    if (size < 1) size = 1;

	return (Max_Records (max_records + size));
}
