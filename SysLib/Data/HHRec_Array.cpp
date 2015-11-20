//*********************************************************
//	HHRec_Array.cpp - household record index arrays
//*********************************************************

#include "HHRec_Array.hpp"

//---------------------------------------------------------
//	HHRec_Array constructor
//---------------------------------------------------------

HHRec_Array::HHRec_Array (int data_size, int max_records, bool sorted) : 
	Index_Array (data_size, max_records, sorted)
{
}

//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int HHRec_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	HHRec_Index *d1 = (HHRec_Index *) data1;
	HHRec_Index *d2 = (HHRec_Index *) data2;

	if (d1->Household () > d2->Household ()) {
		return (1);
	} else if (d1->Household () < d2->Household ()) {
		return (-1);
	} else if (d1->Person () > d2->Person ()) {
		return (1);
	} else if (d1->Person () < d2->Person ()) {
		return (-1);
	} else if (d1->Record () > d2->Record ()) {
		return (1);
	} else if (d1->Record () < d2->Record ()) {
		return (-1);
	}
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

bool HHRec_Array::Split_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (false);

	HHRec_Index *d1 = (HHRec_Index *) data1;
	HHRec_Index *d2 = (HHRec_Index *) data2;

	int prec = Key_Precedent (d1->Household (), d2->Household ());

	if (prec != 0) {
		return (prec > 0);
	}
	prec = Key_Precedent (d1->Person (), d2->Person ());

	if (prec != 0) {
		return (prec > 0);
	}
	prec = Key_Precedent (d1->Record (), d2->Record ());

	return (prec > 0);
}
