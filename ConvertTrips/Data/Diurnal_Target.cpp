//*********************************************************
//	Diurnal_Target.cpp - diurnal target data classes
//*********************************************************

#include "Diurnal_Target.hpp"

//---------------------------------------------------------
//	Diurnal_Target constructor
//---------------------------------------------------------

Diurnal_Target::Diurnal_Target (void) : Diurnal_Data ()
{
	target = NULL;
	trips = NULL;
}

bool Diurnal_Target::Num_Shares (int num)
{
	size_t size = (size_t) num * sizeof (double);

	target = (double *) realloc (target, size);

	if (target == NULL) return (false);

	size = (size_t) num * sizeof (int);

	trips = (int *) realloc (trips, size);

	if (trips == NULL) return (false);

	return (Diurnal_Data::Num_Shares (num));
}

void Diurnal_Target::Clear (void)
{
	Diurnal_Data::Clear ();

	if (target != NULL) {
		delete [] target;
		target = NULL;
	}
	if (trips != NULL) {
		delete [] trips;
		trips = NULL;
	}
}

//---------------------------------------------------------
//	Diurnal_Target_Array constructor
//---------------------------------------------------------

Diurnal_Target_Array::Diurnal_Target_Array (int max_records) : 
	Diurnal_Array (max_records)
{
	Data_Size (sizeof (Diurnal_Target));
}

void Diurnal_Target_Array::Clear (void)
{
	for (Diurnal_Target *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}
