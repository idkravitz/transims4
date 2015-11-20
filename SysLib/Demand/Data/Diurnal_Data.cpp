//*********************************************************
//	Diurnal_Data.cpp - diurnal distribution data classes
//*********************************************************

#include "Diurnal_Data.hpp"

//---------------------------------------------------------
//	Diurnal_Data constructor
//---------------------------------------------------------

Diurnal_Data::Diurnal_Data (void)
{
	share = NULL;
	start = end = nshare = 0;
}

bool Diurnal_Data::Num_Shares (int num)
{
	size_t size = (size_t) num * sizeof (double);

	share = (double *) realloc (share, size);

	if (share == NULL) return (false);

	nshare = num;
	return (true);
}

void Diurnal_Data::Clear (void)
{
	if (share != NULL) {
		free (share);
		share = NULL;
	}
}

//---------------------------------------------------------
//	Diurnal_Array constructor
//---------------------------------------------------------

Diurnal_Array::Diurnal_Array (int max_records) : 
	Data_Array (sizeof (Diurnal_Data), max_records)
{
}

void Diurnal_Array::Clear (void)
{
	for (Diurnal_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}
