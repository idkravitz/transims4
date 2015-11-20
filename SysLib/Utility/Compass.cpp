//*********************************************************
//	Compass.cpp - compass directions
//*********************************************************

#include "Compass.hpp"

#include "System_Defines.hpp"

#include <stdlib.h>
#include <math.h>

char * Compass_Points::point_text [] = {
	"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"
};

//---------------------------------------------------------
//	Compass_Points constructor
//---------------------------------------------------------

Compass_Points::Compass_Points (int num_points)
{
	Set_Points (num_points);
}

//---------------------------------------------------------
//	Compass_Points destructor
//---------------------------------------------------------

Compass_Points::~Compass_Points (void)
{
}

//---------------------------------------------------------
//	Set_Points
//---------------------------------------------------------

void Compass_Points::Set_Points (int num_points)
{
	compass_points = num_points;
	compass_break = num_points / 2;
}

//---------------------------------------------------------
//	Direction
//---------------------------------------------------------

int Compass_Points::Direction (double dx, double dy)
{
	int point;
	double bearing;

	bearing = atan2 (dx, dy) * RAD2DEG;

	if (bearing < 0.0) bearing += 360.0;

	point = (int) (compass_points * bearing / 360.0 + 0.5);

	if (point == compass_points) point = 0;

	return (point);
}

//---------------------------------------------------------
//	Difference
//---------------------------------------------------------

int Compass_Points::Difference (int dir, int dir2)
{
	int diff;

	diff = abs (dir2 - dir);
	if (diff > compass_break) {
		diff = compass_points - diff;
	}
	return (diff);
}

//---------------------------------------------------------
//	Change
//---------------------------------------------------------

int Compass_Points::Change (int dir, int dir2)
{
	int diff;

	diff = dir2 - dir;
	if (diff >= compass_break) {
		diff = diff - compass_points;
	} else if (diff < -compass_break) {
		diff = diff + compass_points;
	}
	return (diff);
}

//---------------------------------------------------------
//	Flip
//---------------------------------------------------------

int Compass_Points::Flip (int dir)
{
	dir -= compass_break;
	if (dir < 0) dir += compass_points;

	return (dir);
}

//---------------------------------------------------------
//	Direction_Text
//---------------------------------------------------------

char * Compass_Points::Direction_Text (double dx, double dy)
{
	int point = Direction (dx, dy);

	if (compass_points > 0) {
		point = (16 * point + compass_points / 2) / compass_points;
	}
	return (point_text [point]);
}

//---------------------------------------------------------
//	Point_Text
//---------------------------------------------------------

char * Compass_Points::Point_Text (int dir)
{
	if (compass_points > 0) {
		dir = (16 * dir + compass_points / 2) / compass_points;
	}
	if (dir < 0) {
		dir = 0;
	} else if (dir > 15) {
		dir = 15;
	}
	return (point_text [dir]);
}
