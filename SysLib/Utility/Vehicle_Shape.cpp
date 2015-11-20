//*********************************************************
//	Vehicle_Shape.cpp - vehicle polygon function
//*********************************************************

#include "Vehicle_Shape.hpp"

#include <stdlib.h>
#include <math.h>

bool Vehicle_Shape (XYZ_Point p1, XYZ_Point p2, double width, Point_Data *points)
{
	double x0, y0, z0, dx, dy, dz, length;

	XYZ_Point p;

	width /= 2.0;

	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	dz = p2.z - p1.z;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	x0 = p1.x + 1.5 * dx;
	y0 = p1.y + 1.5 * dy;
	z0 = p1.z = 1.5 * dz;

	points->Reset ();
	if (!points->Add (&p1)) return (false);

	p.x = x0 - width * dy;
	p.y = y0 + width * dx;
	p.z = z0;

	if (!points->Add (&p)) return (false);

	p.x = p2.x - width * dy;
	p.y = p2.y + width * dx;
	p.z = p2.z;

	if (!points->Add (&p)) return (false);

	p.x = p2.x + width * dy;
	p.y = p2.y - width * dx;
	p.z = p2.z;

	if (!points->Add (&p)) return (false);

	p.x = x0 + width * dy;
	p.y = y0 - width * dx;
	p.z = z0;

	if (!points->Add (&p)) return (false);

	return (points->Add (&p1));
}

//---------------------------------------------------------
//	Vehicle_Shape
//---------------------------------------------------------

bool Vehicle_Shape (XYZ_Point *pt, int num_pts, double width, Point_Data *points)
{
	if (num_pts == 2) {
		return (Vehicle_Shape (pt [0], pt [1], width, points));
	}
	int i;
	double x0, y0, z0, length, dx1, dy1, dx2, dy2;

	XYZ_Point p, p1, p2;

	width /= 2.0;

	points->Reset ();
	p1 = pt [0];
	p2 = pt [1];

	if (!points->Add (&p1)) return (false);

	dx1 = p2.x - p1.x;
	dy1 = p2.y - p1.y;

	length = sqrt (dx1 * dx1 + dy1 * dy1);

	if (length > 0) {
		dx1 /= length;
		dy1 /= length;
	}
	x0 = p1.x + 1.5 * dx1;
	y0 = p1.y + 1.5 * dy1;
	z0 = p1.z;

	p.x = x0 - width * dy1;
	p.y = y0 + width * dx1;
	p.z = z0;

	if (!points->Add (&p)) return (false);

	x0 += width * dy1;
	y0 -= width * dx1;
	dx2 = dy2 = 0;

	for (i=2; i < num_pts; i++) {
		p1 = p2;
		p2 = pt [i];

		dx2 = p2.x - p1.x;
		dy2 = p2.y - p1.y;

		length = sqrt (dx2 * dx2 + dy2 * dy2);

		if (length > 0) {
			dx2 /= length;
			dy2 /= length;
		}
		dx1 = (dx1 + dx2) / 2.0;
		dy1 = (dy1 + dy2) / 2.0;

		p.x = p1.x - width * dy1;
		p.y = p1.y + width * dx1;
		p.z = p1.z;
			
		if (!points->Add (&p)) return (false);

		dx1 = dx2;
		dy1 = dy2;
	}
	p.x = p2.x - width * dy2;
	p.y = p2.y + width * dx2;
	p.z = p2.z;

	if (!points->Add (&p)) return (false);

	p.x = p2.x + width * dy2;
	p.y = p2.y - width * dx2;
	p.z = p2.z;

	if (!points->Add (&p)) return (false);

	p2 = pt [num_pts - 2];
	dx1 = -dx2;
	dy1 = -dy2;

	for (i=num_pts - 3; i >= 0; i--) {
		p1 = p2;
		p2 = pt [i];

		dx2 = p2.x - p1.x;
		dy2 = p2.y - p1.y;

		length = sqrt (dx2 * dx2 + dy2 * dy2);

		if (length > 0) {
			dx2 /= length;
			dy2 /= length;
		}
		dx1 = (dx1 + dx2) / 2.0;
		dy1 = (dy1 + dy2) / 2.0;

		p.x = p1.x - width * dy1;
		p.y = p1.y + width * dx1;
		p.z = p1.z;
			
		if (!points->Add (&p)) return (false);

		dx1 = dx2;
		dy1 = dy2;
	}
	p.x = x0;
	p.y = y0;
	p.z = z0;

	if (!points->Add (&p)) return (false);

	return (points->Add (&pt [0]));
}