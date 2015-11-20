//*********************************************************
//	Class_Pointer.cpp - class pointer arrays
//*********************************************************

#include "Class_Pointer.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	Class_Pointer constructor
//---------------------------------------------------------

Class_Pointer::Class_Pointer (int max_records, bool sorted) :
	Index_Pointer_Array (max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Class_Pointer::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	Class_Index *d1, *d2;

	d1 = *((Class_Index **) data1);
	d2 = *((Class_Index **) data2);

	if (d1 == NULL || d2 == NULL) return (0);

	if (d1->ID () > d2->ID ()) return (1);
	if (d1->ID () < d2->ID ()) return (-1);
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

bool Class_Pointer::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	Class_Index *d1, *d2;
	
	d1 = *((Class_Index **) data1);
	d2 = *((Class_Index **) data2);

	if (d1 == NULL || d2 == NULL) return (false);

	return (Key_Precedent (d1->ID (), d2->ID ()) > 0);
}

//---------------------------------------------------------
//	Class2_Pointer constructor
//---------------------------------------------------------

Class2_Pointer::Class2_Pointer (int max_records, bool sorted) :
	Index_Pointer_Array (max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Class2_Pointer::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	Class2_Index *d1, *d2;

	d1 = *((Class2_Index **) data1);
	d2 = *((Class2_Index **) data2);

	if (d1 == NULL || d2 == NULL) return (0);

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

bool Class2_Pointer::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	Class2_Index *d1, *d2;
	
	d1 = *((Class2_Index **) data1);
	d2 = *((Class2_Index **) data2);

	if (d1 == NULL || d2 == NULL) return (false);

	int prec = Key_Precedent (d1->Key1 (), d2->Key1 ());

	if (prec != 0) {
		return (prec > 0);
	}
	prec = Key_Precedent (d1->Key2 (), d2->Key2 ());

	return (prec > 0);
}
