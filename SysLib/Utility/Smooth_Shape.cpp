//*********************************************************
//	Smooth_Shape.cpp -- smooth shape points
//*********************************************************

#include "Smooth_Shape.hpp"

#include "Compass.hpp"

#include <math.h>

//---------------------------------------------------------
//	Smooth_Shape
//---------------------------------------------------------

bool Smooth_Shape (Point_Data *points, int max_angle, int min_length)
{
	if (points->Num_Points () < 3) return (false);

	int loop, dir1, dir2, diff, max_angle15;
	double x1, y1, z1, x2, y2, z2, dx, dy, dz, len1, len2, x0, y0, z0, factor, min_len, min_len2;
	bool first_flag, second_flag, loop_flag, change_flag;

	XYZ_Point point, *point_ptr, *old_ptr;

	Point_Data current;

	current.Z_Flag (points->Z_Flag ());

	Compass_Points compass (360);

	//---- calculate shape length ----

	if (max_angle < 5) max_angle = 5;
	if (min_length < 1) min_length = 1;

	min_len = min_length;
	min_length = (int) (min_len * 10.0 + 0.5);

	max_angle15 = (int) (max_angle * 1.5);
	min_len2 = min_length * 2.0;

	change_flag = false;
	loop_flag = true;
	x1 = y1 = z1 = len1 = 0.0;
	dir1 = 0;

	for (loop=0; loop < 100 && loop_flag; loop++) {
		if (points->Num_Points () < 3) break;

		//---- copy the existing points ----

		current.Reset ();

		for (point_ptr = points->First (); point_ptr; point_ptr = points->Next ()) {
			current.Add (point_ptr);
		}
		points->Reset ();

		first_flag = second_flag = true;
		loop_flag = false;

		for (point_ptr = current.First (); point_ptr; point_ptr = current.Next ()) {

			x2 = point_ptr->x;
			y2 = point_ptr->y;
			z2 = point_ptr->z;

			if (first_flag) {
				first_flag = false;
				len2 = 0.0;
				dir2 = 0;
			} else {
				dx = x2 - x1;
				dy = y2 - y1;
				dz = z2 - z1;

				dir2 = compass.Direction (dx, dy);
				len2 = sqrt (dx * dx + dy * dy + dz * dz);

				//---- discard very short shapes ----

				if (len2 < min_len && current.Record_Index () < current.Num_Points ()) continue;

				if (second_flag) {
					second_flag = false;
				} else {
					diff = compass.Difference (dir1, dir2);

					//---- adjust the difference for short distances ----

					if (len2 < min_length) {
						if (len2 > 0.0) {
							diff = (int) (diff * min_length / len2);
						} else {
							diff = 360;
						}
					}

					//---- smooth large direction changes ----

					if (diff > max_angle) {
						loop_flag = change_flag = true;

						if (len2 < min_length && current.Record_Index () < current.Num_Points ()) {

							if (diff <= max_angle15) {

								//---- move the last point ----

								old_ptr = points->Last ();

								old_ptr->x = x2 = (x1 + x2) / 2.0;
								old_ptr->y = y2 = (y1 + y2) / 2.0;
								old_ptr->z = z2 = (z1 + z2) / 2.0;

								points->Set ();

								old_ptr = points->Previous ();

								dx = x2 - old_ptr->x;
								dy = y2 - old_ptr->y;
								dz = z2 - old_ptr->z;

								dir1 = compass.Direction (dx, dy);
								len1 = sqrt (dx * dx + dy * dy + dz * dz);
								x1 = x2;
								y1 = y2;
								z1 = z2;
							}
							continue;

						} else if (len1 < min_len2 || len2 < min_len2) {

							if (diff >= max_angle15) {

								//---- discard the last point ----

								old_ptr = points->Last ();

								old_ptr->x = point_ptr->x;
								old_ptr->y = point_ptr->y;
								old_ptr->z = point_ptr->z;

								points->Set ();

								old_ptr = points->Previous ();

								dx = x2 - old_ptr->x;
								dy = y2 - old_ptr->y;
								dz = z2 - old_ptr->z;

								dir1 = compass.Direction (dx, dy);
								len1 = sqrt (dx * dx + dy * dy + dz * dz);
								x1 = x2;
								y1 = y2;
								z1 = z2;
								continue;

							} else if (len1 >= min_len2) {

								//---- move the last point ----

								old_ptr = points->Point (points->Num_Points () - 1);

								x0 = old_ptr->x;
								y0 = old_ptr->y;
								z0 = old_ptr->z;

								factor = min_length / len1;

								old_ptr = points->Last ();

								old_ptr->x = x1 = x1 + (x0 - x1) * factor;
								old_ptr->y = y1 = y1 + (y0 - y1) * factor;
								old_ptr->z = z1 = z1 + (z0 - z1) * factor;

								points->Set ();

								dx = x2 - x1;
								dy = y2 - y1;
								dz = z2 - z1;

								dir2 = compass.Direction (dx, dy);
								len2 = sqrt (dx * dx + dy * dy + dz * dz);

							} else if (len2 >= min_len2) {

								//---- add a shape point ----

								factor = min_length / len2;
								len2 = len2 - min_length;

								point.x = x1 + (x2 - x1) * factor;
								point.y = y1 + (y2 - y1) * factor;
								point.z = z1 + (z2 - z1) * factor;

								points->Add (&point);

							} else {

								//---- move the last point ----

								old_ptr = points->Point (points->Num_Points () - 1);

								x0 = old_ptr->x;
								y0 = old_ptr->y;
								z0 = old_ptr->z;

								old_ptr = points->Last ();

								old_ptr->x = x1 = (x0 + x2 + x1 + x1) / 4.0;
								old_ptr->y = y1 = (y0 + y2 + y1 + y1) / 4.0;
								old_ptr->z = z1 = (z0 + z2 + z1 + z1) / 4.0;

								points->Set ();

								dx = x2 - x1;
								dy = y2 - y1;
								dz = z2 - z1;

								dir2 = compass.Direction (dx, dy);
								len2 = sqrt (dx * dx + dy * dy + dz * dz);
							}

						} else {

							//---- round the shape ----

							old_ptr = points->Point (points->Num_Points () - 1);

							x0 = old_ptr->x;
							y0 = old_ptr->y;
							z0 = old_ptr->z;

							factor = min_length / len1;

							old_ptr = points->Last ();

							old_ptr->x = x1 + (x0 - x1) * factor;
							old_ptr->y = y1 + (y0 - y1) * factor;
							old_ptr->z = z1 + (z0 - z1) * factor;

							points->Set ();

							factor = min_length / len2;
							len2 = len2 - min_length;

							point.x = x1 + (x2 - x1) * factor;
							point.y = y1 + (y2 - y1) * factor;
							point.z = z1 + (z2 - z1) * factor;

							points->Add (&point);
						}
					}
				}
			}
			points->Add (point_ptr);

			x1 = x2;
			y1 = y2;
			z1 = z2;
			len1 = len2;
			dir1 = dir2;
		}
	}
	return (change_flag);
}
