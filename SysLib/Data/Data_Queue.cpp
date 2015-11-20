//*********************************************************
//	Data_Queue.cpp - managed queue of data pointers
//*********************************************************

#include "Data_Queue.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Data_Queue constructor
//---------------------------------------------------------

Data_Queue::Data_Queue (int _max_records)
{
	queue = NULL;
	num_records = max_records = record_index = num_removed = 0;

	Max_Records (_max_records);
}

//---------------------------------------------------------
//	Add
//---------------------------------------------------------

bool Data_Queue::Add (void *data)
{
	if (data == NULL) return (false);

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

void * Data_Queue::First (void)
{
	void **ptr = queue;

	if (num_removed > num_records / 2) {
		Compress ();
	}
	for (record_index = 0; record_index < num_records; record_index++, ptr++) {
		if (*ptr != NULL) {
			return (*ptr);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Next
//---------------------------------------------------------

void * Data_Queue::Next (void)
{
	if (++record_index < num_records) {
		void **ptr = queue + record_index;

		for (; record_index < num_records; record_index++, ptr++) {
			if (*ptr != NULL) {
				return (*ptr);
			}
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Remove
//---------------------------------------------------------

void Data_Queue::Remove (bool delete_flag)
{
	if (record_index < num_records) {
		void **ptr = queue + record_index;

		if (delete_flag && *ptr != NULL) {
			delete [] ((char *) *ptr);
		}
		*ptr = NULL;
		num_removed++;
	}
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Data_Queue::Clear (void)
{
	if (queue != NULL) {
		for (record_index=0; record_index < num_records; record_index++) {
			Remove ();
		}
		free (queue); 
		queue = NULL; 
	}
	num_records = max_records = num_removed = 0;
}

//---------------------------------------------------------
//	Compress
//---------------------------------------------------------

void Data_Queue::Compress (void)
{
	if (num_removed > 0) {
		int num_new = 0;
		int new_index = 0;
		void **new_ptr, **old_ptr;

		new_ptr = old_ptr = queue;

		for (int i=0; i < num_records; i++, old_ptr++) {
			if (*old_ptr != NULL) {
				if (i == record_index) new_index = num_new;

				*new_ptr++ = *old_ptr;
				num_new++;
			}
		}
		num_records = num_new;
		record_index = new_index;
		num_removed = 0;
	}
}

//---------------------------------------------------------
//	Max_Records
//---------------------------------------------------------

bool Data_Queue::Max_Records (int max_size)
{
	if (max_size == 0) {
		Clear ();
		return (true);
	}
	if (max_size < 0) return (false);

	int num = num_records;
	num_records = max_records = 0;

	size_t size = max_size * sizeof (void *);

	queue = (void **) realloc (queue, size);

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

bool Data_Queue::Add_Memory (void)
{
	int size = (1 << 16) / sizeof (void *);
	
	if (max_records > 0 && max_records < size) {
		size = max_records >> 1;
	}
    if (size < 1) size = 1;

	return (Max_Records (max_records + size));
}
