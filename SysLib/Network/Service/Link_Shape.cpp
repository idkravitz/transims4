//*********************************************************
//	Link_Shape.cpp -- follow the link shape between offsets
//*********************************************************

#include "Network_Service.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Shape
//---------------------------------------------------------

bool Network_Service::Link_Shape (Link_Data *link_ptr, int dir, Point_Data *points, double offset, double length, double side)
{
	int i, j, num_pts;
	double xa, ya, za, xb, yb, zb, factor, link_len, x1, y1, z1, x2, y2, z2;
	double dx, dy, dz, dx1, dy1, dz1, dx2, dy2, dz2, end_offset;
	bool point_flag, flip_flag, start_flag, first_flag;

	XYZ *pt_ptr;
	XYZ_Point point, *point_ptr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	point_flag = (length == 0.0);
	flip_flag = (dir == 1);
	x1 = y1 = z1 = x2 = y2 = z2 = dx2 = dy2 = dz2 = 0.0;

	//---- get the link length and node coordinates ----

	link_len = UnRound (link_ptr->Length ());
	factor = 1.0;

	//---- check the offsets ----

	if (offset == -1.0) {
		offset = 0.0;
	} else if (flip_flag) {
		offset = link_len - offset;
	}
	if (length == -1.0) length = link_len;

	end_offset = offset + length;

	if (offset < 0.0) {
		offset = 0.0;
	} else if (offset > link_len) {
		offset = link_len;
	}
	if (end_offset < 0.0) {
		end_offset = 0.0;
	} else if (end_offset > link_len) {
		end_offset = link_len;
	}

	//---- get the end nodes ----

	node_ptr = node_data.Get ((flip_flag) ? link_ptr->Bnode () : link_ptr->Anode ());

	xa = UnRound (node_ptr->X ());
	ya = UnRound (node_ptr->Y ());
	za = UnRound (node_ptr->Z ());

	node_ptr = node_data.Get ((flip_flag) ? link_ptr->Anode () : link_ptr->Bnode ());

	xb = UnRound (node_ptr->X ());
	yb = UnRound (node_ptr->Y ());
	zb = UnRound (node_ptr->Z ());

	dx = xb - xa;
	dy = yb - ya;
	dz = zb - za;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	//---- get the shape record ----

	if (Network_File_Flag (SHAPE)) {
		shape_ptr = shape_data [link_ptr->Shape ()];
	} else {
		shape_ptr = NULL;
	}
	points->Reset ();

	//---- process offsets for a simple link ----

	if (shape_ptr == NULL) {
		i = (point_flag) ? 1 : 2;
		if (!points->Max_Points (i)) goto mem_error;

		if (link_len > 0.0) {
			factor = length / link_len;
		}
		offset *= factor;

		point.x = xa + offset * dx + side * dy;
		point.y = ya + offset * dy - side * dx;
		point.z = za + offset * dz;

		if (!points->Add (&point)) goto mem_error;

		if (point_flag) return (true);

		end_offset *= factor;

		point.x = xa + end_offset * dx + side * dy;
		point.y = ya + end_offset * dy - side * dx;
		point.z = za + end_offset * dz;

		if (!points->Add (&point)) goto mem_error;

		return (true);
	}

	//---- create the link vector ----

	num_pts = 2 + shape_ptr->Points ();

	if (!points->Max_Points (num_pts)) goto mem_error;

	j = (flip_flag) ? shape_ptr->Points () : 1;

	if (side == 0.0) {

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				point.x = xa;
				point.y = ya;
				point.z = za;
			} else if (i == num_pts) {
				point.x = xb;
				point.y = yb;
				point.z = zb;
			} else {
				pt_ptr = shape_ptr->Point (j);

				point.x = UnRound (pt_ptr->x);
				point.y = UnRound (pt_ptr->y);
				point.z = UnRound (pt_ptr->z);

				j += (flip_flag) ? -1 : 1;
			}
			if (!points->Add (&point)) goto mem_error;
		}

	} else {
		dx1 = dy1 = dz1 = 0;

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				x2 = xa;
				y2 = ya;
				z2 = z1;
				dx2 = dy2 = dz2 = 0.0;
			} else {
				if (i == num_pts) {
					x2 = xb;
					y2 = yb;
					z2 = zb;
				} else {
					pt_ptr = shape_ptr->Point (j);

					x2 = UnRound (pt_ptr->x);
					y2 = UnRound (pt_ptr->y);
					z2 = UnRound (pt_ptr->z);

					j += (flip_flag) ? -1 : 1;
				}
				dx2 = x2 - x1;
				dy2 = y2 - y1;
				dz2 = z2 - z1;

				length = sqrt (dx2 * dx2 + dy2 * dy2 + dz2 * dz2);

				if (length == 0.0) {
					dx2 = dx;
					dy2 = dy;
					dz2 = dz;
				} else {
					dx2 /= length;
					dy2 /= length;
					dz2 /= length;
				}
				if (i == 2) {
					dx1 = dx2;
					dy1 = dy2;
					dz1 = dz2;
				} else {
					dx1 = (dx1 + dx2) / 2.0;
					dy1 = (dy1 + dy2) / 2.0;
					dz1 = (dz1 + dz2) / 2.0;
				}
				point.x = x1 + side * dy1;
				point.y = y1 - side * dx1;
				point.z = z1;

				if (!points->Add (&point)) goto mem_error;

				if (i == num_pts) {
					point.x = x2 + side * dy2;
					point.y = y2 - side * dx2;
					point.z = z2;

					if (!points->Add (&point)) goto mem_error;
				}
			}
			x1 = x2;
			y1 = y2;
			z1 = z2;
			dx1 = dx2;
			dy1 = dy2;
			dz1 = dz2;
		}
	}

	if (offset == 0.0 && end_offset == link_len) return (true);

	//---- calculate shape length ----

	length = 0.0;
	first_flag = true;

	for (point_ptr = points->First (); point_ptr; point_ptr = points->Next ()) {

		x2 = point_ptr->x;
		y2 = point_ptr->y;
		z2 = point_ptr->z;

		if (first_flag) {
			first_flag = false;
		} else {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += sqrt (dx * dx + dy * dy + dz * dz);
		}
		x1 = x2;
		y1 = y2;
		z1 = z2;
	}

	//---- process offsets for a shape link ----

	if (length == 0.0) length = 0.01;

	if (link_len > 0.0) {
		factor = length / link_len;
	}
	offset *= factor;
	end_offset *= factor;

	num_pts = 0;
	length = 0.0;
	start_flag = false;
	first_flag = true;

	for (point_ptr = points->First (); point_ptr; point_ptr = points->Next ()) {

		x2 = point_ptr->x;
		y2 = point_ptr->y;
		z2 = point_ptr->z;

		if (first_flag) {
			first_flag = false;

			if (offset == 0.0) {
				start_flag = true;
				num_pts++;
			}
		} else {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += link_len = sqrt (dx * dx + dy * dy + dz * dz);

			if (!start_flag) {
				if (length >= offset) {
					if (length > offset) {
						if (link_len > 0.0) {
							factor = (offset + link_len - length) / link_len;
						} else {
							factor = 1.0;
						}
						point.x = x1 + dx * factor;
						point.y = y1 + dy * factor;
						point.z = z1 + dz * factor;
						points->Set (++num_pts, &point);
					} else {
						points->Set (++num_pts);
					}
					start_flag = true;
					if (point_flag) break;
				}
			}
			if (start_flag) {
				if (length <= end_offset) {
					if (length == offset) break;
					points->Set (++num_pts);
					if (length == end_offset) break;
				} else {
					if (link_len > 0.0) {
						factor = (end_offset + link_len - length) / link_len;
					} else {
						factor = 1.0;
					}
					point.x = x1 + dx * factor;
					point.y = y1 + dy * factor;
					point.z = z1 + dz * factor;

					points->Set (++num_pts, &point);
					break;
				}
			}
		}
		x1 = x2;
		y1 = y2;
		z1 = z2;
	}
	if (num_pts == 0) return (false);

	return (points->Num_Points (num_pts));

mem_error:
	Error ("Insufficient Memory for Shape Points");
	return (false);
}
