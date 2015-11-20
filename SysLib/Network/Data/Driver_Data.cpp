//*********************************************************
//	Driver_Data.cpp - transit driver data classes
//*********************************************************

#include "Driver_Data.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Driver_Data constructor
//---------------------------------------------------------

Driver_Data::Driver_Data (int route) : Class_Index (route)
{
	Type (0);
	Null_Notes ();
	nlinks = 0;
}

void Driver_Data::Clear (void)
{
	if (nlinks > 0) {
		delete [] link;
	}
	nlinks = 0;

	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
}

bool Driver_Data::Links (int value)
{
	if (nlinks > 0) Clear ();
	if (value <= 0) return (true);

	link = new int [value];

	if (link == NULL) return (false);

	nlinks = value;
	memset (link, '\0', nlinks * sizeof (int));
	return (true);
}

bool Driver_Data::Notes (char *value)
{
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
	if (value != NULL) {
		int len = (int) strlen (value) + 1;

		notes = new char [len];
		if (notes == NULL) return (false);

		str_cpy (notes, len, value);
	}
	return (true);
}

//---------------------------------------------------------
//	Driver_Array constructor
//---------------------------------------------------------

Driver_Array::Driver_Array (int max_records) : 
	Class_Array (sizeof (Driver_Data), max_records)
{
	driver_records = 0;
}

Driver_Array::~Driver_Array (void)
{
	for (Driver_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Clear ();
}
