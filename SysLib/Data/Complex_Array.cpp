//*********************************************************
//	Complex_Array.cpp - data array with complex index keys
//*********************************************************

#include "Complex_Array.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Complex_Array constructor
//---------------------------------------------------------

Complex_Array::Complex_Array (int data_size, int num_keys, bool pointer_flag, int max_records, bool sorted) :
	Index_Array ()
{
	Initialize (data_size, num_keys, pointer_flag, max_records, sorted);
}

//---------------------------------------------------------
//	Complex_Array destructor
//---------------------------------------------------------

Complex_Array::~Complex_Array (void)
{
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Complex_Array::Initialize (int data_size, int _num_keys, bool _pointer_flag, int max_records, bool sorted)
{
	num_keys = (_num_keys > 0) ? _num_keys : 1;
	pointer_flag = _pointer_flag;

	return (Index_Array::Initialize (data_size, max_records, sorted));
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Complex_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	int *key1, *key2;

	if (pointer_flag) {
		key1 = *((int **) data1);
		key2 = *((int **) data2);
		if (key1 == NULL || key2 == NULL) return (0);
	} else {
		key1 = (int *) data1;
		key2 = (int *) data2;
	}

	for (int i=0; i < num_keys; i++, key1++, key2++) {
		if (*key1 > *key2) return (1);
		if (*key1 < *key2) return (-1);
	}
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

bool Complex_Array::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	int prec;
	int *key1, *key2;

	if (pointer_flag) {
		key1 = *((int **) data1);
		key2 = *((int **) data2);
		if (key1 == NULL || key2 == NULL) return (false);
	} else {
		key1 = (int *) data1;
		key2 = (int *) data2;
	}

	for (int i=0; i < num_keys; i++, key1++, key2++) {
		prec = Key_Precedent (*key1, *key2);

		if (prec != 0) {
			return (prec > 0);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Delete_Pointers
//---------------------------------------------------------

void Complex_Array::Delete_Pointers (void)
{
	if (!pointer_flag) return;

	void **ptr;

	for (ptr =  (void **) First (); ptr != NULL; ptr = (void **) Next ()) {
		if (*ptr != NULL) {
			delete [] (char *) *ptr;
			*ptr = NULL;
		}
	}
	Data_Array::Reset ();
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Complex_Array::Clear (void)
{
	Delete_Pointers ();

	Index_Array::Clear ();
}

//---------------------------------------------------------
//	Reset
//---------------------------------------------------------

void Complex_Array::Reset (void)
{
	Delete_Pointers ();

	Data_Array::Reset ();
}
