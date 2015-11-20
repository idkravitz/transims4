
#include "ArcNet.hpp"

#include <math.h>

void IntSecPtOfLines ( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double *xM, double *yM ) {

	*xM = ( (x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) )
		/ ( (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4) );
	*yM = ( (x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) )
		/ ( (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4) );

	return;
}

void CartToPolar (double x, double y, double *r, double *alpha) {

	*r = sqrt (x*x+y*y);
	double pi = 4.0 * atan(1.0);
	if ( x == 0.0 && y == 0.0 ) {
		*alpha = 0.0;
	} else if ( x >= 0.0 ) {
		*alpha = asin ( y/(*r) );
	} else {
		*alpha = pi - asin ( y/(*r) );
	}
	if ( *alpha < 0.0 ) {
		*alpha += 2.0 * pi;
	}
}

void PolarToCart (double r, double alpha, double *x, double *y) {

	*x = r * cos(alpha);
	*y = r * sin(alpha);
}

double Distance (double x1, double y1, double x2, double y2) {
	
	double dx = x2-x1;
	double dy = y2-y1;
	return sqrt(dx*dx+dy*dy);
}

double NormalizeAnglePositive (double angle) {

	double pi = 4.0 * atan(1.0);
	int phases;
	if ( angle >= 0 ) {
		phases = int(angle / 2.0 / pi);
		angle -= phases * 2.0 * pi;
	} else {
		phases = int(angle / 2.0 / pi) - 1;
		angle -= phases * 2.0 * pi;
	}
	return angle;
}

double NormalizeAngleCenter (double angle) {

	double pi = 4.0 * atan(1.0);
	angle = NormalizeAnglePositive(angle);
	if ( angle > pi ) angle -= 2.0 * pi;
	return angle;
}

bool IsVeryClose ( double val1, double val2, double eps ) {
	
	double diff = fabs(val2 - val1);
	if ( diff < eps  ) return true;
	return false;
}

bool PtIsOnLine (double x1, double y1, double x2, double y2, double xP, double yP) {

	if ( IsVeryClose(x1, xP, 1.e-10) && IsVeryClose(y1, yP, 1.e-10) ) return true;

	double ratio1 = fabs((x2 - x1) / (y2 - y1));
	double ratio2 = fabs((xP - x1) / (yP - y1));
	return IsVeryClose(ratio1, ratio2, 1.e-5);
}

bool PtIsOnLineSegment (double x1, double y1, double x2, double y2, double xP, double yP) {

	// establish the bounding box
	double xmin = x1;
	double xmax = x1;
	if ( x2 < xmin ) xmin = x2;
	if ( x2 > xmax ) xmax = x2;
	double ymin = y1;
	double ymax = y1;
	if ( y2 < ymin ) ymin = y2;
	if ( y2 > ymax ) ymax = y2;

	// the point has to be in the bounding box
	if ( xP < xmin ) return false;
	if ( xP > xmax ) return false;
	if ( yP < ymin ) return false;
	if ( yP > ymax ) return false;

	return PtIsOnLine (x1, y1, x2, y2, xP, yP);
}

bool RoundedConnectivity (double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, Arcview_File *arcview_file ) {

	XYZ_Point point;

	// calculate the deltas for all simple line segments
	double dxA = x2 - x1;
	double dyA = y2 - y1;
	double drA, alphaA;
	CartToPolar (dxA,dyA,&drA,&alphaA);
	double dxB = x3 - x2;
	double dyB = y3 - y2;
	double drB, alphaB;
	CartToPolar (dxB,dyB,&drB,&alphaB);
	double dxC = x4 - x3;
	double dyC = y4 - y3;
	double drC, alphaC;
	CartToPolar (dxC,dyC,&drC,&alphaC);

	// calculate the intersection point of the incoming and outgoing segments
	double xM, yM;
	IntSecPtOfLines (x1, y1, x2, y2, x3, y3, x4, y4, &xM, &yM);
	double drM2 = Distance (x2, y2, xM, yM);
	double drM3 = Distance (x3, y3, xM, yM);

	// check whether these intersect already (when the intersection is too tight)
	bool ProcessConnection = true;
	if ( PtIsOnLineSegment (x1, y1, x2, y2, xM, yM) ) {
		ProcessConnection = false;
	}
	if ( PtIsOnLineSegment (x3, y3, x4, y4, xM, yM) ) {
		ProcessConnection = false;
	}

	// calculate the intersection point of perpendicular lines at points 2 and 3
	double pi = 4.0 * atan(1.0);
	double dx3N, dy3N, x3N, y3N;
	PolarToCart(drC, alphaC + pi/2.0, &dx3N, &dy3N);
	x3N = x3 + dx3N;
	y3N = y3 + dy3N;
	double dx2N, dy2N, x2N, y2N;
	PolarToCart(drA, alphaA + pi/2.0, &dx2N, &dy2N);
	x2N = x2 + dx2N;
	y2N = y2 + dy2N;
	double xP, yP;
	IntSecPtOfLines (x2, y2, x2N, y2N, x3, y3, x3N, y3N, &xP, &yP);
	double drP2 = Distance (x2, y2, xP, yP);
	double drP3 = Distance (x3, y3, xP, yP);

	// the ratio of the lengths of the two legs to the center of the arc
	double ratio = drP2/drP3;
	if ( drP3/drP2 < ratio ) ratio = drP3/drP2;

	double alphaAB = NormalizeAngleCenter(alphaB - alphaA);
	double alphaBC = NormalizeAngleCenter(alphaC - alphaA);
	double angle_ratio = alphaAB/alphaBC;
	if ( alphaBC/alphaAB < angle_ratio ) angle_ratio = alphaBC/alphaAB;

	if ( ProcessConnection && angle_ratio > 0.2 && angle_ratio < 0.8 && ratio > 0.25 && drM2 < drB*2.0 && drM3 < drB*2.0 && drP2 < 250.0 && drP3 < 250.0 ) {
		double betaP2,betaP3,x,y;
		CartToPolar (xP-x2, yP-y2, &drP2, &betaP2);
		CartToPolar (xP-x3, yP-y3, &drP3, &betaP3);

		double delta = NormalizeAngleCenter(betaP3 - betaP2);

		double beta = betaP2;
		double radius = drP2;
	//	double beta = betaP3;
	//	double radius = drP3;
		int num_pt = 15;
		double exp = 1.0;
		for ( int j=0 ; j<=num_pt ; j++ ) {
			double factor = ( 1.0 + cos(pi*double(j)/num_pt) ) / 2.0;
			beta = betaP2 + delta * j / num_pt;
			radius = drP2 * factor + drP3 * (1.0 - factor);
	//		beta = betaP3 - delta * j / num_pt;
	//		radius = drP3 * factor + drP2 * (1.0 - factor);
			PolarToCart(radius, beta, &x, &y);
			point.x = xP - x;
			point.y = yP - y;
			if (!arcview_file->points.Add (&point)) goto mem_error;
		}
	} else {

		// this is the default behavior as used before rounded connectivity
		point.x = x2;
		point.y = y2;
		if (!arcview_file->points.Add (&point)) goto mem_error;
		point.x = x3;
		point.y = y3;
		if (!arcview_file->points.Add (&point)) goto mem_error;
	}
	return true;

mem_error:
	return (false);
}
