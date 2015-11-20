//*********************************************************
//	Shape_Data.cpp - shape point data classes
//*********************************************************

#include "Shape_Data.hpp"

//---------------------------------------------------------
//	Shape_Data constructor
//---------------------------------------------------------

Shape_Data::Shape_Data (int link) : Class_Index (link)
{
	point = new XYZ_Array ();
}

bool Shape_Data::Points (int value)
{
	if (!point) {
		point = new XYZ_Array ();
	}
	if (point) {
		return (point->Num_Points (value)); 
	} else {
		return (false);
	}
}

bool Shape_Data::Max_Points (int value)
{
	if (!point) {
		point = new XYZ_Array ();
	}
	if (point) {
		return (point->Max_Points (value)); 
	} else {
		return (false);
	}
}

bool Shape_Data::Point (int num, int *x, int *y, int *z)
{
	XYZ *pt_ptr = Point (num);

	if (pt_ptr != NULL) {
		*x = pt_ptr->x;
		*y = pt_ptr->y;
		if (z != NULL) *z = pt_ptr->z;
		return (true);
	} else {
		*x = *y = 0;
		if (z != NULL) *z = 0;
		return (false);
	}
}

//---------------------------------------------------------
//	Shape_Array constructor
//---------------------------------------------------------

Shape_Array::Shape_Array (int max_records) : 
	Class_Array (sizeof (Shape_Data), max_records)
{
}

Shape_Array::~Shape_Array (void)
{
	for (Shape_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Clear ();
}
