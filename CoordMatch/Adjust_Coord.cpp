//*********************************************************
//	Adjust_Coord.cpp - adjust the coordinates
//*********************************************************

#include "CoordMatch.hpp"

#include <math.h>

//---------------------------------------------------------
//	Adjust_Coord
//---------------------------------------------------------

void CoordMatch::Adjust_Coord (int *x, int *y)
{
	int i, num;
	double dx, dy, distance, min_distance [4];

	Node_Map *map_ptr, *best_ptr [4];

	for (i=0; i < 4; i++) {
		min_distance [i] = 0.0;
		best_ptr [i] = NULL;
	}
	num = 0;

	for (map_ptr = (Node_Map *) node_map.First (); map_ptr; map_ptr = (Node_Map *) node_map.Next ()) {
		if (++num > num_match) break;

		dx = (double) map_ptr->x - *x;
		dy = (double) map_ptr->y - *y;

		distance = dx * dx + dy * dy;

		if (distance == 0.0) {
			*x += map_ptr->dx;
			*y += map_ptr->dy;
			return;
		}

		if (dx >= 0.0) {
			if (dy >= 0.0) {
				i = 0;
			} else {
				i = 1;
			}
		} else if (dy >= 0.0) {
			i = 3;
		} else {
			i = 2;
		}
		if (best_ptr [i] == NULL || distance < min_distance [i]) {
			best_ptr [i] = map_ptr;
			min_distance [i] = distance;
		}
	}
	distance = 0.0;

	for (i=0; i < 4; i++) {
		if (min_distance [i] > 0.0) {
            distance += min_distance [i] = 1.0 / sqrt (min_distance [i]);
		}
	}
	if (distance > 0.0) {
		for (i=0; i < 4; i++) {
			if (best_ptr [i] != NULL) {
				*x += (int) (best_ptr [i]->dx * min_distance [i] / distance + 0.5);
				*y += (int) (best_ptr [i]->dy * min_distance [i] / distance + 0.5);
			}
		}
	}
	return;
}
