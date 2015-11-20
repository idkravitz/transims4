//*********************************************************
//	Attribute_Data.cpp - household attribute data class
//*********************************************************

#include "Attribute_Data.hpp"

#include <string.h>
#include <math.h>
#include <stdio.h>

//---------------------------------------------------------
//	Attribute_Data constructor
//---------------------------------------------------------

Attribute_Data::Attribute_Data (int types)
{
	num_types = num_cells = 0;
	target = NULL;
	total = NULL;
	index = NULL;

	Num_Types (types);
}

//---- allocate memory for attribute data ----

bool Attribute_Data::Num_Types (int types)
{
	Clear_Types ();

	if (types > 0) {
		total = new double [types];
		target = new double [types];

		if (total == NULL || target == NULL) return (false);

		memset (total, '\0', types * sizeof (double));
		memset (target, '\0', types * sizeof (double));
		num_types = types;
	}
	return (true);
}

//---- allocate memory for cell matrix ----

bool Attribute_Data::Num_Cells (int cells)
{
	Clear_Cells ();

	if (cells > 0) {
		index = new int [cells];
		if (index == NULL) return (false);

		memset (index, '\0', cells * sizeof (int));

		num_cells = cells;
	}
	return (true);
}

//---- clear attribute memory ----

void Attribute_Data::Clear_Types (void)
{
	if (num_types > 0) {
		if (total != NULL) {
			delete total;
			total = NULL;
		}
		if (target != NULL) {
			delete target;
			target = NULL;
		}
		num_types = 0;
	}
}

//---- clear cell matrix ----

void Attribute_Data::Clear_Cells (void)
{
	if (num_cells > 0) {
		if (index != NULL) {
			delete index;
			index = NULL;
		}
		num_cells = 0;
	}
}

//---- clear all memory ----

void Attribute_Data::Clear (void)
{
	Clear_Types ();
	Clear_Cells ();
}

//---- normalize target ----

void Attribute_Data::Normalize (void)
{
	int type;
	double *ptr;
	double total = 0.0;

	for (type=0, ptr=target; type < num_types; type++, ptr++) {
		total += *ptr;
	}
	if (total > 0.0 && total != 1.0) {
		for (type=0, ptr=target; type < num_types; type++, ptr++) {
			*ptr /= total;
		}
	}
}

//---- zero the attribute totals ----

void Attribute_Data::Zero_Total (void)
{
	if (num_types > 0) {
		memset (total, '\0', num_types * sizeof (double));
	}
}

//---- calculate adjustment factors ----

double Attribute_Data::Factor_Total (void)
{
	int type;
	double *ptr1, *ptr2, diff;

	diff = 0.0;

	for (type=0, ptr1=total, ptr2=target; type < num_types; type++, ptr1++, ptr2++) {
		diff += fabs (*ptr1 - *ptr2);
		if (*ptr1 != 0.0) *ptr1 = *ptr2 / *ptr1;
	}
	return (diff);
}


//---------------------------------------------------------
//	Attribute_Array constructor
//---------------------------------------------------------

Attribute_Array::Attribute_Array (int max_records) : 
	Data_Array (sizeof (Attribute_Data), max_records)
{
}

//---- free attribute memory ----

void Attribute_Array::Clear (void)
{
	for (Attribute_Data *at_ptr = First (); at_ptr; at_ptr = Next ()) {
		at_ptr->Clear ();
	}
}
