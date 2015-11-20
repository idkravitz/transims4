//*********************************************************
//	Point_Array.cpp - dynamic point array
//*********************************************************

#include "Point_Array.hpp"

//---------------------------------------------------------
//	Point_Data constructor
//---------------------------------------------------------

Point_Data::Point_Data (int max_points, bool flag) 
{
	xy_data.Max_Points (max_points);

	z_flag = flag;
	if (z_flag) {
		z_data.Max_Points (max_points);
	}
}

//---- Point ----

XYZ_Point * Point_Data::Point (int num) 
{
	XY_Point *ptr = xy_data.Point (num);
	if (ptr == NULL) return (NULL);

	pt.x = ptr->x;
	pt.y = ptr->y;
	pt.z = 0.0;

	if (z_flag) {
		double *z = z_data.Point (num);
		if (z) pt.z = *z;
	}
	return (&pt);
}

//---- First ----

XYZ_Point * Point_Data::First (void) 
{
	XY_Point *ptr = xy_data.First ();
	if (ptr == NULL) return (NULL);

	pt.x = ptr->x;
	pt.y = ptr->y;
	pt.z = 0.0;

	if (z_flag) {
		double *z = z_data.First ();
		if (z) pt.z = *z;
	}
	return (&pt);
}

//---- Next ----

XYZ_Point * Point_Data::Next (void) 
{
	XY_Point *ptr = xy_data.Next ();
	if (ptr == NULL) return (NULL);

	pt.x = ptr->x;
	pt.y = ptr->y;
	pt.z = 0.0;

	if (z_flag) {
		double *z = z_data.Next ();
		if (z) pt.z = *z;
	}
	return (&pt);
}

//---- Last ----

XYZ_Point * Point_Data::Last (void) 
{
	XY_Point *ptr = xy_data.Last ();
	if (ptr == NULL) return (NULL);

	pt.x = ptr->x;
	pt.y = ptr->y;
	pt.z = 0.0;

	if (z_flag) {
		double *z = z_data.Last ();
		if (z) pt.z = *z;
	}
	return (&pt);
}

//---- Previous ----

XYZ_Point * Point_Data::Previous (void) 
{
	XY_Point *ptr = xy_data.Previous ();
	if (ptr == NULL) return (NULL);

	pt.x = ptr->x;
	pt.y = ptr->y;
	pt.z = 0.0;

	if (z_flag) {
		double *z = z_data.Previous ();
		if (z) pt.z = *z;
	}
	return (&pt);
}

//---- Add ----

bool Point_Data::Add (XYZ_Point *data) 
{
	XY_Point xy;
	double z;

	if (data != NULL) {
		xy.x = data->x;
		xy.y = data->y;
		z = data->z;
	} else {
		xy.x = pt.x;
		xy.y = pt.y;
		z = pt.z;
	}
	if (z_flag) {
		z_data.Add (&z);
	}
	return (xy_data.Add (&xy));
}

//---- Set ----

bool Point_Data::Set (int index, XYZ_Point *data) 
{
	XY_Point xy;

	if (data == NULL) {
		xy.x = pt.x;
		xy.y = pt.y;
	} else {
		xy.x = data->x;
		xy.y = data->y;
	}
	if (!xy_data.Set (index, &xy)) return (false);

	if (z_flag) {
		double z;

		if (data == NULL) {
			z = pt.z;
		} else {
			z = data->z;
		}
		if (!z_data.Set (index, &z)) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Point_Array constructor
//---------------------------------------------------------

Point_Array::Point_Array (int max_records) : 
	Data_Array (sizeof (XY_Point), max_records)
{
}

//---------------------------------------------------------
//	Z_Array constructor
//---------------------------------------------------------

Z_Array::Z_Array (int max_records) : 
	Data_Array (sizeof (double), max_records)
{
}
	
//---------------------------------------------------------
//	XYZ_Array constructor
//---------------------------------------------------------

XYZ_Array::XYZ_Array (int max_records) : 
	Data_Array (sizeof (XYZ), max_records)
{
}
