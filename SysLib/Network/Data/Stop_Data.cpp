//*********************************************************
//	Stop_Data.cpp - network transit stop data classes
//*********************************************************

#include "Stop_Data.hpp"

#include "Utility.hpp"
#include <string.h>

//---------------------------------------------------------
//	Stop_Data constructor
//---------------------------------------------------------

Stop_Data::Stop_Data (int id) : Class_Index (id)
{
	Link_Dir (0);
	Offset (0);
	Use (0);
	Type (0);
	Space (0);
	Traveler (0);
	Null_Name ();
	Null_Notes ();
}

void Stop_Data::Clear (void)
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
}

bool Stop_Data::Name (char *value)
{
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (value != NULL) {
		int len = (int) strlen (value) + 1;

		name = new char [len];
		if (name == NULL) return (false);

		str_cpy (name, len, value);
	}
	return (true);
}

bool Stop_Data::Notes (char *value)
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
//	Stop_Array constructor
//---------------------------------------------------------

Stop_Array::Stop_Array (int max_records) : 
	Class_Array (sizeof (Stop_Data), max_records, true)
{
}

Stop_Array::~Stop_Array (void)
{
	for (Stop_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Clear ();
}
