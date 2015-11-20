//*********************************************************
//	Route_Data.cpp - network transit line data classes
//*********************************************************

#include "Route_Data.hpp"

#include "Utility.hpp"
#include <string.h>

//---------------------------------------------------------
//	Route_Data constructor
//---------------------------------------------------------

Route_Data::Route_Data (int id) : Class_Index (id)
{
	Mode (0);
	Min_Time (0);
	Reset ();
	Null_Name ();
	Null_Notes ();
}

int  Route_Data::n = 0;
int  Route_Data::p = 0;

void Route_Data::Clear (void)
{
	if (nodes > 0) {
		delete [] node;
		node = NULL;
	}
	if (periods > 0) {
		delete [] period;
		period = NULL;
	}
	nodes = periods = 0;

	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
	if (notes != NULL) {
		delete [] notes;
		notes = NULL;
	}
}

bool Route_Data::Periods (int value)
{
	if (periods > 0) {
		delete [] period;
		period = NULL;
	}
	if (value <= 0) return (true);

	period = new Period_Data [value];

	if (period == NULL) return (false);

	periods = value;
	memset (period, '\0', periods * sizeof (Period_Data));
	return (true);
}

bool Route_Data::Nodes (int value)
{
	if (nodes > 0) {
		delete [] node;
		node = NULL;
	}
	if (value <= 0) return (true);

	node = new Node_Data [value];

	if (node == NULL) return (false);

	nodes = value;
	memset (node, '\0', nodes * sizeof (Node_Data));
	return (true);
}

bool Route_Data::Name (char *value)
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

bool Route_Data::Notes (char *value)
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
//	Route_Array constructor
//---------------------------------------------------------

Route_Array::Route_Array (int max_records) : 
	Class_Array (sizeof (Route_Data), max_records, false)
{
}

Route_Array::~Route_Array (void)
{
	for (Route_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Clear ();
}
