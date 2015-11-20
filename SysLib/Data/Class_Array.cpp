//*********************************************************
//	Class_Array.cpp - class data arrays
//*********************************************************

#include "Class_Array.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Class_Array constructor
//---------------------------------------------------------

Class_Array::Class_Array (int data_size, int max_records, bool sorted) :
	Index_Array (data_size, max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Class_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	if (((Class_Index *) data1)->ID ()	
			> ((Class_Index *) data2)->ID ()) {
		return (1);
	} else if (((Class_Index *) data1)->ID ()	
			< ((Class_Index *) data2)->ID ()) {
		return (-1);
	}
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

inline bool Class_Array::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	return (Key_Precedent (((Class_Index *) data1)->ID (), ((Class_Index *) data2)->ID ()) > 0);
}

//---------------------------------------------------------
//	Class2_Array constructor
//---------------------------------------------------------

Class2_Array::Class2_Array (int data_size, int max_records, bool sorted) :
	Complex_Array (data_size, 2, false, max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Class2_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	Class2_Index *d1 = (Class2_Index *) data1;
	Class2_Index *d2 = (Class2_Index *) data2;

	if (d1->Key1 () > d2->Key1 ()) {
		return (1);
	} else if (d1->Key1 () < d2->Key1 ()) {
		return (-1);
	} else if (d1->Key2 () > d2->Key2 ()) {
		return (1);
	} else if (d1->Key2 () < d2->Key2 ()) {
		return (-1);
	}
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

bool Class2_Array::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	Class2_Index *d1 = (Class2_Index *) data1;
	Class2_Index *d2 = (Class2_Index *) data2;

	int prec = Key_Precedent (d1->Key1 (), d2->Key1 ());

	if (prec != 0) {
		return (prec > 0);
	}
	prec = Key_Precedent (d1->Key2 (), d2->Key2 ());

	return (prec > 0);
}
