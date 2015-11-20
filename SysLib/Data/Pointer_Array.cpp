//*********************************************************
//	Pointer_Array.cpp - array of pointers to index classes
//*********************************************************

#include "Pointer_Array.hpp"

//---------------------------------------------------------
//	Pointer_Array constructor
//---------------------------------------------------------

Pointer_Array::Pointer_Array (void)
{
	data_ptr = new void *;
}

//---------------------------------------------------------
//	Pointer_Array destructor
//---------------------------------------------------------

Pointer_Array::~Pointer_Array (void)
{
	if (data_ptr != NULL) {
		delete data_ptr;
		data_ptr = NULL;
	}
}

//---------------------------------------------------------
//	Data_Pointer_Array constructor
//---------------------------------------------------------

Data_Pointer_Array::Data_Pointer_Array (int max_records) :
	Data_Array (sizeof (void *), max_records)
{
}

//---------------------------------------------------------
//	Delete_Pointers
//---------------------------------------------------------

void Data_Pointer_Array::Delete_Pointers (void)
{
	void *ptr;

	for (ptr = First (); ptr != NULL; ptr = Next ()) {
		delete [] (char *) ptr;
		ptr = NULL;
	}
	Reset ();
}

//---------------------------------------------------------
//	Index_Pointer_Array constructor
//---------------------------------------------------------

Index_Pointer_Array::Index_Pointer_Array (int max_records, bool sorted) :
	Index_Array (sizeof (void *), max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Index_Pointer_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	int *d1, *d2;

	d1 = *((int **) data1);
	d2 = *((int **) data2);

	if (d1 == NULL || d2 == NULL) return (0);

	if (*d1 > *d2) return (1);
	if (*d1 < *d2) return (-1);
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

bool Index_Pointer_Array::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	int *d1, *d2;
	
	d1 = *((int **) data1);
	d2 = *((int **) data2);

	if (d1 == NULL || d2 == NULL) return (false);

	return (Key_Precedent (*d1, *d2) > 0);
}

//---------------------------------------------------------
//	Delete_Pointers
//---------------------------------------------------------

void Index_Pointer_Array::Delete_Pointers (void)
{
	void *ptr;

	for (ptr = First (); ptr != NULL; ptr = Next ()) {
		delete [] (char *) ptr;
		ptr = NULL;
	}
	Reset ();
}

