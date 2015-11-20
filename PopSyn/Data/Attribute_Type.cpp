//*********************************************************
//	Attribute_Type.cpp - attribute type class
//*********************************************************

#include "Attribute_Type.hpp"

#include "Utility.hpp"
#include <math.h>

//---------------------------------------------------------
//	Attribute_Type constructor
//---------------------------------------------------------

Attribute_Type::Attribute_Type (int types)
{
	num_types = pums_field = offset = type = 0;
	pums_breaks = NULL;
	zone_field = NULL;
	name = NULL;

	Num_Types (types);
}

//---- allocate memory for attribute data ----

bool Attribute_Type::Num_Types (int types)
{
	Clear_Types ();

	if (types > 0) {
		zone_field = new int [types];

		if (zone_field == NULL) return (false);

		memset (zone_field, '\0', types * sizeof (int));
		num_types = types;
	}
	return (true);
}

//---- clear attribute memory ----

void Attribute_Type::Clear_Types (void)
{
	if (num_types > 0) {
		if (zone_field != NULL) {
			delete zone_field;
			zone_field = NULL;
		}
		num_types = 0;
	}
}

//---- clear all memory ----

void Attribute_Type::Clear (void)
{
	Clear_Types ();

	if (pums_breaks != NULL) {
		delete pums_breaks;
		pums_breaks = NULL;
	}
	if (name != NULL) {
		delete [] name;
		name = NULL;
	}
}

//---- field name ----

void Attribute_Type::Field_Name (char *text)
{
	name = NULL;

	if (text == NULL) return;

	int len = (int) strlen (text);
	name = new char [len + 1];

	if (name != NULL) {
		str_cpy (name, len+1, text);
	}
}

//---------------------------------------------------------
//	Attribute_Type_Array constructor
//---------------------------------------------------------

Attribute_Type_Array::Attribute_Type_Array (int max_records) : 
	Data_Array (sizeof (Attribute_Type), max_records)
{
}

//---- free attribute memory ----

void Attribute_Type_Array::Clear (void)
{
	for (Attribute_Type *at_ptr = First (); at_ptr; at_ptr = Next ()) {
		at_ptr->Clear ();
	}
}
