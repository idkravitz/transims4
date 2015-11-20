//*********************************************************
//	In_Polygon.cpp - Check if a Point in in a Polygon
//*********************************************************

#include "In_Polygon.hpp"

#include <stdlib.h>

//---------------------------------------------------------
//	X_Sort_Function
//---------------------------------------------------------

int X_Sort_Function (const void *rec1, const void *rec2)
{
	double r1, r2;

	r1 = *((double *) rec1);
	r2 = *((double *) rec2);

	if (r1 > r2) return (1);
	if (r1 < r2) return (-1);
	return (0);
}

//---------------------------------------------------------
//	In_Polygon
//---------------------------------------------------------

bool In_Polygon (double x, double y, Point_Data *points)
{
	int num;
	double x1, y1, x2, y2, xlist [100];
	bool first;

	XYZ_Point *point_ptr;
	
	if (points->Num_Points () < 3) return (false);

	num = 0;
	first = true;
	x1 = y1 = 0.0;

	for (point_ptr = points->First (); point_ptr != NULL && num < 100; point_ptr = points->Next ()) {
		x2 = point_ptr->x;
		y2 = point_ptr->y;

		if (first) {
			first = false;
		} else {
			if (y1 == y2) {
				if (y == y1) {
					if (x1 < x2) {
						if (x1 <= x && x <= x2) return (true);
					} else {
						if (x2 <= x && x <= x1) return (true);
					}
				}
			} else if (y1 < y2) {
				if (y1 <= y && y < y2) {
					xlist [num++] = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
				}
			} else {
				if (y2 < y && y <= y1) {
					xlist [num++] = x2 + (y - y2) * (x1 - x2) / (y1 - y2);
				}
			}
		}
		x1 = x2;
		y1 = y2;
	}
	if (num < 2) return (false);

	if (num == 2) {
		if (xlist [0] < xlist [1]) {
			if (xlist [0] <= x && x <= xlist [1]) return (true);
		} else {
			if (xlist [1] <= x && x <= xlist [0]) return (true);
		}
	} else {
		qsort (xlist, num, sizeof (double), X_Sort_Function);

		for (int i=0; i < num; i+=2) {
			if (xlist [i] <= x && x <= xlist [i+1]) return (true);
		}
	}
	return (false);
}
