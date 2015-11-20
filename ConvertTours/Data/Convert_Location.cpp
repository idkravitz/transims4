//*********************************************************
//	Convert_Location.cpp - additional location data
//*********************************************************

#include "Convert_Location.hpp"

#include <memory.h>

//---------------------------------------------------------
//	Convert_Location constructor
//---------------------------------------------------------

int Convert_Location::num_weight = 0;

Convert_Location::Convert_Location (int id) : Location_Data (id)
{
	weight = NULL;

	Initialize ();
}

Convert_Location::~Convert_Location (void)
{
	if (weight != NULL) {
		delete [] weight;
	}
}

bool Convert_Location::Initialize (void)
{
	Zone_List (0);
	Org_Parking (0);
	Des_Parking (0);

	if (num_weight <= 0) return (true);

	weight = new double [num_weight];

	if (weight == NULL) return (false);

	memset (weight, '\0', num_weight * sizeof (double));
	return (true);
}

//---------------------------------------------------------
//	Convert_Location_Array constructor
//---------------------------------------------------------

Convert_Location_Array::Convert_Location_Array (int max_records) : 
	Class_Array (sizeof (Convert_Location), max_records, true)
{
}
