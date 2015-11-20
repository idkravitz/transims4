
void IntSecPtOfLines ( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double *xM, double *yM );

void CartToPolar (double x, double y, double *r, double *alpha);

void PolarToCart (double r, double alpha, double *x, double *y);

double Distance (double x1, double y1, double x2, double y2);
	
double NormalizeAnglePositive (double angle);

double NormalizeAngleCenter (double angle);

bool IsVeryClose ( double val1, double val2, double eps );

bool PtIsOnLine (double x1, double y1, double x2, double y2, double xP, double yP);

bool PtIsOnLineSegment (double x1, double y1, double x2, double y2, double xP, double yP);

bool RoundedConnectivity (double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, Arcview_File *arcview_file );

