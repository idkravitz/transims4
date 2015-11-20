//*********************************************************
//	Add_Arrow.cpp - additonal an arrow to a link
//*********************************************************

#include "ArcNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Arrow
//---------------------------------------------------------

void ArcNet::Add_Arrow (Point_Data *points)
{
	XYZ_Point point, end_point, *point_ptr;

	double dx, dy, dz, length;

	point_ptr = points->Last ();
	end_point = *point_ptr;

	for (point_ptr = points->Previous (); point_ptr; point_ptr = points->Previous ()) {
		dx = end_point.x - point_ptr->x;
		dy = end_point.y - point_ptr->y;
		dz = end_point.z - point_ptr->z;

		length = dx * dx + dy * dy;

		if (length >= arrow_length * arrow_length) {
			length = sqrt (length);
			dx /= length;
			dy /= length;
			dz /= length;
			break;
		}
	}
	if (point_ptr == NULL) return;
	
	point.x = end_point.x - arrow_length * dx + arrow_side * dy;
	point.y = end_point.y - arrow_length * dy - arrow_side * dx;
	point.z = end_point.z - arrow_length * dz;

	if (!points->Add (&point)) return;

	point.x = end_point.x - arrow_length * dx - arrow_side * dy;
	point.y = end_point.y - arrow_length * dy + arrow_side * dx;

	if (!points->Add (&point)) return;

	points->Add (&end_point);
}
