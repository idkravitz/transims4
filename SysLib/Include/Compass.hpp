//*********************************************************
//	Compass.hpp - compass orientation
//*********************************************************

#ifndef COMPASS_HPP
#define COMPASS_HPP

//---------------------------------------------------------
//	Compass_Points constructor
//---------------------------------------------------------

class Compass_Points
{
public:
	Compass_Points (int num_points = 8);
	virtual ~Compass_Points (void);

	void Set_Points (int num_points);
	int  Direction (double dx, double dy);
	int  Difference (int dir, int dir2);
	int  Change (int dir, int dir2);
	int  Flip (int dir);

	char *Direction_Text (double dx, double dy);
	char *Point_Text (int dir);

	int  Num_Points (void)			{ return (compass_points); }

private:
	int  compass_points;
	int  compass_break;

	static char *point_text [];
 };

#endif

