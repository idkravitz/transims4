//*********************************************************
//	Equation.cpp - volume-delay equation array
//*********************************************************

#include "Equation.hpp"

#include "Utility.hpp"

#include <math.h>

//---------------------------------------------------------
//	Equation_Array constructor
//---------------------------------------------------------

Equation_Array::Equation_Array (int max_records) : 
	Data_Array (sizeof (Equation), max_records), Static_Service ()
{
}

//---------------------------------------------------------
//	Equation_Array destructor
//---------------------------------------------------------

Equation_Array::~Equation_Array (void)
{
}

//---------------------------------------------------------
//	Add_Equation
//---------------------------------------------------------

bool Equation_Array::Add_Equation (int type, char *parameters)
{
	static char *eq_text [] = {"BPR", "BPR_PLUS", "EXPONENTIAL", "CONICAL", "AKCELIK", "BPR+", "EXP", "CON", NULL};
	static Equation_Type eq_map [] = { BPR, BPR_PLUS, EXP, CONICAL, AKCELIK, BPR_PLUS, EXP, CONICAL };

	Equation *eq_ptr = (Equation *) Record (type);

	if (eq_ptr == NULL) return (false);

	if (parameters != NULL) {
		int i;
		char buffer [FIELD_BUFFER];

		parameters = Get_Token (parameters, buffer);

		for (i=0; eq_text [i] != NULL; i++) {
			if (str_cmp (buffer, eq_text [i]) == 0) {
				eq_ptr->type = eq_map [i];
				break;
			}
		}
		if (eq_text [i] == NULL) {
			if (Send_Messages ()) {
				exe->Error ("Unrecognized Equation Type = %s", buffer);
			}
			return (false);
		}
		parameters = Get_Double (parameters, &(eq_ptr->a));
		parameters = Get_Double (parameters, &(eq_ptr->b));
		parameters = Get_Double (parameters, &(eq_ptr->c));
		parameters = Get_Double (parameters, &(eq_ptr->d));

		if (eq_ptr->type == BPR || eq_ptr->type == BPR_PLUS) {
			if (eq_ptr->a < 0.01 || eq_ptr->b < 1.0 || eq_ptr->c < 0.4) {
				if (Send_Messages ()) {
					exe->Error ("BPR Equation Parameters %.2lf, %.2lf, %.2lf are Out of Range", eq_ptr->a, eq_ptr->b, eq_ptr->c);
				}
				return (false);
			}
			if (eq_ptr->type == BPR_PLUS) {
				if (eq_ptr->d <= 0.0) eq_ptr->d = 1.0;
				if (eq_ptr->d > 7.0) {
					if (Send_Messages ()) {
						exe->Error ("BPR Plus Equation Minimum Speed %.2lf is Out of Range", eq_ptr->d);
					}
					return (false);
				}
			}
		} else if (eq_ptr->type == EXP) {
			if (eq_ptr->a < 0.07 || eq_ptr->a > 7.5 || eq_ptr->b < 1.0 || eq_ptr->b > 20.0 ||
				eq_ptr->c < 30.0 || eq_ptr->c > 750.0) {
				if (Send_Messages ()) {
					exe->Error ("Exponential Equation Parameters %.2lf, %.2lf, %.1lf are Out of Range", eq_ptr->a, eq_ptr->b, eq_ptr->c);
				}
			}
		} else if (eq_ptr->type == CONICAL) {
			if (eq_ptr->a <= 1.0 || eq_ptr->a > 20.0) {
				if (Send_Messages ()) {
					exe->Error ("Conical Equation Parameter %.2lf is Out of Range (1..20)", eq_ptr->a);
				}
				return (false);
			}
			eq_ptr->b = (2 * eq_ptr->a - 1) / (2 * eq_ptr->a - 2);
		} else if (eq_ptr->type == AKCELIK) {
			if (eq_ptr->a <= 0.05 || eq_ptr->a > 2.5) {
				if (Send_Messages ()) {
					exe->Error ("AKCELIK Equation Parameter *.2lf is Out of Range (0.05..2.5)", eq_ptr->a);
				}
				return (false);
			}
		}
	} else if (type == 1) {

		eq_ptr->type = BPR;
		eq_ptr->a = 0.15;
		eq_ptr->b = 4.0;
		eq_ptr->c = 0.75;
		eq_ptr->d = 0.0;

	} else {

		Equation *prev_ptr = (Equation *) Record (type-1);

		if (prev_ptr == NULL) return (false);

		*eq_ptr = *prev_ptr;
		return (true);
	}

	if (Send_Messages ()) {
		exe->Print (1, "Equation Parameters %d = %s, A=%0.2lf",
			type, eq_text [eq_ptr->type], eq_ptr->a);

		if (eq_ptr->type != CONICAL && eq_ptr->type != AKCELIK) {
			exe->Print (0, ", B=%0.2lf, C=%.2lf", eq_ptr->b, eq_ptr->c);
		}
		if (eq_ptr->type == BPR_PLUS) {
			exe->Print (0, ", D=%.2lf", eq_ptr->d);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Apply_Equation
//---------------------------------------------------------

int Equation_Array::Apply_Equation (int type, int time0, int volume, int capacity, int length)
{
	if (volume <= 0) {
		return (time0);
	} else {
		int time;
		double cap, len;
		Equation *eq_ptr = (Equation *) Record (type);

		if (eq_ptr == NULL) return (time0);

		if (capacity < 1) capacity = 1;
		cap = capacity;

		if (eq_ptr->type == BPR || eq_ptr->type == BPR_PLUS) {
			cap *= eq_ptr->c;
			if (cap < 1.0) cap = 1.0;

			time = (int) (time0 * (1.0 + eq_ptr->a * pow ((volume / cap), eq_ptr->b)) + 0.5);

			if (eq_ptr->type == BPR_PLUS) {
				int max_time = (int) (length / eq_ptr->d + 0.5);
				if (time > max_time) time = max_time;
			}
		} else if (eq_ptr->type == EXP) {
			len = length / 1000.0;

			time = (int) (time0 + len * MIN (eq_ptr->a * exp (eq_ptr->b * (volume / cap)), eq_ptr->c) + 0.5);

		} else if (eq_ptr->type == CONICAL) {
			cap = 1 - (volume / cap);

			time = (int) (time0 * (2 - eq_ptr->b - eq_ptr->a * cap + 
				sqrt (eq_ptr->a * eq_ptr->a * cap * cap + eq_ptr->b * eq_ptr->b)) + 0.5);
		} else if (eq_ptr->type == AKCELIK) {
			double vc, fac, spd;
			vc = volume / cap;
			fac = vc - 1;
			spd = (double) 1.0 / (length / time0 * MPSTOMPH);

			time = (int) (time0 + ((spd + 0.25 * 0.25 * (fac + sqrt ((fac * fac) + (8 * eq_ptr->a * vc)/cap)))) / spd + 0.5);
		} else {
			return (time0);
		}
		if (time < time0) {
			if (time < 0) {
				return (MAX_INTEGER);
			} else {
				return (time0);
			}
		}
		return (time);
	}
}
