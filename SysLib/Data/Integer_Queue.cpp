//*********************************************************
//	Integer_Queue.cpp - managed queue of integers
//*********************************************************

#include "Integer_Queue.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Integer_Queue constructor
//---------------------------------------------------------

Integer_Queue::Integer_Queue (int _max_records)
{
	queue = NULL;
	num_records = max_records = record_index = num_removed = 0;

	Max_Records (_max_records);
}

//---------------------------------------------------------
//	Add
//---------------------------------------------------------

bool Integer_Queue::Add (int data)
{
	if (data == 0) return (false);

	if (num_records >= max_records) {
		if (num_removed > 0) {
			Compress ();
		}
		if (num_records >= max_records) {
			if (!Add_Memory ()) return (false);
		}
	}
	queue [num_records++] = data;
	return (true);
}

//---------------------------------------------------------
//	First
//---------------------------------------------------------

int Integer_Queue::First (void)
{
	int *ptr = queue;

	if (num_removed > num_records / 2) {
		Compress ();
	}
	for (record_index = 0; record_index < num_records; record_index++, ptr++) {
		if (*ptr != 0) {
			return (*ptr);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Next
//---------------------------------------------------------

int Integer_Queue::Next (void)
{
	if (++record_index < num_records) {
		int *ptr = queue + record_index;

		for (; record_index < num_records; record_index++, ptr++) {
			if (*ptr != 0) {
				return (*ptr);
			}
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Remove
//---------------------------------------------------------

void Integer_Queue::Remove (void)
{
	if (record_index < num_records) {
		queue [record_index] = 0;
		num_removed++;
	}
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Integer_Queue::Clear (void)
{
	if (queue != NULL) {
		free (queue); 
		queue = NULL; 
	}
	num_records = max_records = num_removed = 0;
}

//---------------------------------------------------------
//	Compress
//---------------------------------------------------------

void Integer_Queue::Compress (void)
{
	if (num_removed > 0) {
		int num_new = 0;
		int *new_ptr, *old_ptr;

		new_ptr = old_ptr = queue;

		for (int i=0; i < num_records; i++, old_ptr++) {
			if (*old_ptr != 0) {
				*new_ptr++ = *old_ptr;
				num_new++;
			}
		}
		num_records = num_new;
		num_removed = 0;
	}
}

//---------------------------------------------------------
//	Max_Records
//---------------------------------------------------------

bool Integer_Queue::Max_Records (int max_size)
{
	if (max_size == 0) {
		Clear ();
		return (true);
	}
	if (max_size < 0) return (false);

	int num = num_records;
	num_records = max_records = 0;

	size_t size = max_size * sizeof (int);

	queue = (int *) realloc (queue, size);

	if (queue == NULL) return (false);

	max_records = max_size;

	if (num > max_records) {
		num_records = max_records;
	} else {
		num_records = num;
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Memory
//---------------------------------------------------------

bool Integer_Queue::Add_Memory (void)
{
	int size = (1 << 16) / sizeof (int);
	
	if (max_records > 0 && max_records < size) {
		size = max_records >> 1;
	}
    if (size < 1) size = 1;

	return (Max_Records (max_records + size));
}
