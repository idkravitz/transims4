//*********************************************************
//	Projection.cpp - Coordinate Projection Class
//*********************************************************

#include "Projection.hpp"

#include <string.h>
#include <stdlib.h>
#include <math.h>

//---------------------------------------------------------
//	Projection constructor
//---------------------------------------------------------

Projection::Projection (void) : Projection_Code ()
{
	status = convert_flag = false;
}

Projection::Projection (Projection_Data input, Projection_Data output)
{
	Set_Projection (input, output);
}

//---------------------------------------------------------
//	Set_Projection
//---------------------------------------------------------

bool Projection::Set_Projection (Projection_Data _input, Projection_Data _output)
{
	input = _input;
	output = _output;

	status = (input.Status () && output.Status ());
	convert_flag = (input.Type () != NO_PROJECTION && output.Type () != NO_PROJECTION);

	return (status);
}

//---------------------------------------------------------
//	Convert - with x and y parameters
//---------------------------------------------------------

bool Projection::Convert (double *x, double *y)
{
	if (!status) return (false);

	if (input.adjust) {
		*x += input.X_Offset ();
		*y += input.Y_Offset ();
		*x *= input.X_Factor ();
		*y *= input.Y_Factor ();
	}

	if (convert_flag) {
		
		switch (input.Type ()) {

			case LATLONG:	//---- degrees ----

				if (input.Units () == MILLION_DEG) {
					*x /= 1000000.0;
					*y /= 1000000.0;
				}
				break;

			case STATEPLANE:	//---- feet ----

				switch (input.Units ()){
					case METERS:
						*x *= METERTOFEET;
						*y *= METERTOFEET;
						break;
					case MILES:
						*x *= MILETOFEET;
						*y *= MILETOFEET;
						break;
					case KILOMETERS:
						*x *= METERTOFEET * 1000.0;
						*y *= METERTOFEET * 1000.0;
						break;
					default:
						break;
				}
				SPToLatLong (&input, x, y);
				break;

			case UTM:	//---- meters ----

				switch(input.Units ()) {
					case FEET:
						*x /= METERTOFEET;
						*y /= METERTOFEET;
						break;
					case MILES:
						*x *= MILETOMETER;
						*y *= MILETOMETER;
						break;
					case KILOMETERS:
						*x *= 1000.0;
						*y *= 1000.0;
						break;
					default:
						break;
				}
				UtmToLatLong (&input, x, y);
				break;

			default:
				break;
		}

		switch (output.Type ()) {

			case LATLONG:		//---- degrees ----
				if (output.Units () == MILLION_DEG) {
					*x *= 1000000.0;
					*y *= 1000000.0;
				}
				break;

			case STATEPLANE:

				LatLongToSP (&output, x, y);	//---- feet ----

				switch(output.Units ()) {	
					case METERS:
						*x /= METERTOFEET;
						*y /= METERTOFEET;
						break;
					case MILES:
						*x /= MILETOFEET;	//---- MILETOMETER; 
						*y /= MILETOFEET;	//---- MILETOMETER; 
						break;
					case KILOMETERS:
						*x /= METERTOFEET * 1000.0;
						*y /= METERTOFEET * 1000.0;
						break;
					default:
						break;
				}
				break;

			case UTM:

				LatLongToUtm (&output, x, y);		//---- meters ----

				switch(output.Units ()) {
					case FEET:
						*x *= METERTOFEET;
						*y *= METERTOFEET;
						break;
					case MILES:
						*x /= MILETOMETER;
						*y /= MILETOMETER;
						break;
					case KILOMETERS:
						*x /= 1000.0;
						*y /= 1000.0;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	
	if (output.adjust) {
		*x += output.X_Offset ();
		*y += output.Y_Offset ();
		*x *= output.X_Factor ();
		*y *= output.Y_Factor ();
	}
	return (true);
}

//---------------------------------------------------------
//	Length_Factor
//---------------------------------------------------------

double Projection::Length_Factor (void)
{
	double factor;

	if (!status) return (1.0);

	factor = 1.0;

	switch (input.Units ()){
		case FEET:
			switch (output.Units ()) {
				case METERS:
					factor /= METERTOFEET;
					break;
				case MILES:
					factor /= MILETOFEET;
					break;
				case KILOMETERS:
					factor /= METERTOFEET / 1000.0;
					break;
				default:
					break;
			}
			break;

		case METERS:
			switch (output.Units ()) {
				case FEET:
					factor *= METERTOFEET;
					break;
				case MILES:
					factor *= METERTOFEET / MILETOFEET;
					break;
				case KILOMETERS:
					factor /= 1000.0;
					break;
				default:
					break;
			}
			break;

		case MILES:
			switch (output.Units ()) {
				case FEET:
					factor *= MILETOFEET;
					break;
				case METERS:
					factor *= MILETOFEET / METERTOFEET;
					break;
				case KILOMETERS:
					factor *= MILETOFEET / METERTOFEET / 1000.0;
					break;
				default:
					break;
			}
			break;

		case KILOMETERS:
			switch (output.Units ()) {
				case FEET:
					factor *= METERTOFEET * 1000.0;
					break;
				case METERS:
					factor /= 1000.0;
					break;
				case MILES:
					factor *= METERTOFEET * 1000.0 / MILETOFEET;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return (factor);
}

//---------------------------------------------------------
//	Speed_Factor
//---------------------------------------------------------

double Projection::Speed_Factor (void)
{
	double factor;

	if (!status) return (1.0);

	factor = 1.0;

	switch (input.Units ()){
		case FEET:
		case MILES:
			switch (output.Units ()) {
				case METERS:
					factor = MPHTOMPS;
					break;
				case KILOMETERS:
					factor = MPHTOMPS * 3600.0 / 1000.0;
					break;
				default:
					break;
			}
			break;

		case METERS:
			switch (output.Units ()) {
				case FEET:
				case MILES:
					factor = MPSTOMPH;
					break;
				case KILOMETERS:
					factor = 3600.0 / 1000.0;
					break;
				default:
					break;
			}
			break;

		case KILOMETERS:
			switch (output.Units ()) {
				case FEET:
				case MILES:
					factor = MPSTOMPH * 1000.0 / 3600.0;
					break;
				case METERS:
					factor = 1000.0 / 3600.0;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return (factor);
}

//---------------------------------------------------------
//	LatLong to UTM Forward Calculations Are Here
//---------------------------------------------------------

void Projection::LatLongToUtm (Projection_Data *data, double *x, double *y)
{
	double T, C, A, V, M;

	*x *= DEG2RAD;
	*y *= DEG2RAD;

	//---- Compute the Intermediate Variables Here ----

	T = tan (*y);
	T *= T;

	C = cos (*y);
	A = (*x - data->long_origin) * C;

	C = data->e_square * C * C / (1 - data->e_square);

	V = sin (*y);
	V = data->a / sqrt (1 - (data->e_square * V * V));

	M = data->a * (data->a1 * *y - 
		data->a2 * sin (2.0 * *y) + 
		data->a3 * sin (4.0 * *y) - 
		data->a4 * sin (6.0 * *y));

	*x = data->false_easting + data->scale_factor * V *
		(A + (1 - T + C) * (pow (A, 3) / 6) + (5 - 18 * T + pow (T, 2) + 72 * C -
		58 * data->e_prime_sq) * (pow (A, 5) / 120));

	*y = data->false_northing + data->scale_factor *
		(M - data->mo + V * tan (*y) *
		((pow (A, 2) / 2) + (5 - T + 9 * C + 4 * pow (C, 2)) * (pow (A, 4) / 24) +
		(61 - 58 * T + pow (T, 2) + 600 * C -
		330 * data->e_prime_sq) * (pow (A, 6) / 720)));
}

//---------------------------------------------------------
//	UTM to LatLong Reverse Calculations Are Here
//---------------------------------------------------------

void Projection::UtmToLatLong (Projection_Data *data, double *x, double *y)
{
	double a1, a2, a3, a4 ;
	double psi1, v1, rho1, C1, T1, M1, mu1, D;

	M1 = data->mo + ((*y - data->false_northing) / data->scale_factor);
	
	mu1 = M1 / (data->a * (1 - (data->e_square / 4) -
		  (3 * pow (data->e_square, 2) / 64) -
		  (5 * pow (data->e_square, 3) / 256)));

	a1 = ((3 * data->e1 / 2) - (27 * pow (data->e1, 3) / 32)) * sin (2 * mu1);
	a2 = ((21 * pow (data->e1, 2) / 16) - (55 * pow (data->e1, 4) / 32)) * sin (4 * mu1);
	a3 = (151 * pow (data->e1, 3) / 96) * sin (6 * mu1);
	a4 = (1097 * pow (data->e1, 4) / 512) * sin (8 * mu1);

	psi1 = mu1 + a1 + a2 + a3 + a4;
	
	v1 = data->a / sqrt (1 - data->e_square * sin (psi1) * sin (psi1));

	D = (*x - data->false_easting) / (v1 * data->scale_factor);

	rho1 = data->a * (1 - data->e_square) /
		pow ((1 - data->e_square * sin (psi1) * sin (psi1)), 1.5);

	C1 = data->e_prime_sq * cos (psi1) * cos (psi1);

	T1 = tan (psi1) * tan (psi1);

	*x = data->long_origin + (D - ((1 + 2 * T1 + C1) * (pow (D, 3) / 6)) +
			(5 - 2 * C1 + 28 * T1 - 3 * pow (C1, 2) +
			8 * pow (data->e_prime_sq, 2) + 24 * pow (T1, 2)) * (pow (D, 5) / 120)) / cos (psi1);

	*y = psi1 - ((v1 * tan (psi1) / rho1) * ((pow (D, 2) / 2) - (5 + 3 * T1 + 10 * C1 -
			4 * pow (C1, 2) - 9 * pow (data->e_prime_sq, 2)) * (pow (D, 4) / 24) +
			(61 + 90 * T1 + 298 * C1 + 45 * pow (T1, 2) -
			252 * data->e_prime_sq - 3 * pow (C1, 2)) * (pow (D, 6) / 720)));
	
	*x *= RAD2DEG;
	*y *= RAD2DEG;
}

//---------------------------------------------------------
//	LatLong to SP Forward Calculations Are Here
//---------------------------------------------------------

void Projection::LatLongToSP (Projection_Data *data, double *x, double *y)
{
	if (data->scale_factor == 0) {
		double t, theta, r;

		*x *= DEG2RAD;
		*y *= DEG2RAD;

		t = tan ((PI / 4) - (*y / 2)) / 
			pow (((1 - data->e * sin (*y)) /
			(1 + data->e * sin (*y))), data->e / 2);

		r = data->a * data->sf * pow (t, data->n);

		theta = data->n * (*x - data->long_origin);
			
		*x = data->false_easting + r * sin (theta);

		*y = data->false_northing + data->rf - r * cos (theta);

	} else {

		LatLongToUtm (data, x, y);
		*x *= METERTOFEET;
		*y *= METERTOFEET;
	}
}

//---------------------------------------------------------
//	 SP to LatLong Reverse Calculations Are Here
//---------------------------------------------------------

void Projection::SPToLatLong (Projection_Data *data, double *x, double *y)
{
	if (data->scale_factor == 0) {
		double rprime, tprime, thetaprime;
		double lat_trial, lat_diff ;

		thetaprime = atan ((*x - data->false_easting) /
			(data->rf - (*y - data->false_northing)));

		rprime = (data->n / fabs (data->n)) *
				sqrt (pow ((*x - data->false_easting), 2) +
				pow ((data->rf - (*y - data->false_northing)), 2));

		tprime = pow ((rprime / (data->a * data->sf)), (1 / data->n));

		*x = (thetaprime / data->n) + data->long_origin;
			
		// Trial Lat

		lat_trial = (PI / 2) - 2 * atan (tprime);
		*y = 0.00;
		lat_diff = lat_trial - *y;
		
		//--- iterates while the condition is true ----

		while (fabs (lat_diff) > 1e-12) {
			lat_trial = *y;		//---- Has the old value of Latitude ----
			
			//---- compute New Latitude ----

			*y = (PI / 2) - 2 * atan (tprime * 
					(pow (((1 - data->e * sin (*y)) /
					(1 + data->e * sin (*y))), data->e / 2)));

			lat_diff = lat_trial - *y;
		}
		*x *= RAD2DEG;
		*y *= RAD2DEG;

	} else {

		*x /= METERTOFEET;
		*y /= METERTOFEET;
		UtmToLatLong(data, x, y);
	}
}
