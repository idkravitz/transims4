//********************************************************* 
//	Function.cpp - Math Functions
//*********************************************************

#include "User_Program.hpp"

#include <math.h>

//---------------------------------------------------------
//	Function
//---------------------------------------------------------

bool User_Program::Function (void)
{
	double val = 0.0;

	if (s == NULL) {
		if (cmd_ptr->token != RANDOM) {
			exe->Error ("Function Code %d has No Parameters", cmd_ptr->token);
			return (false);
		}
	} else {
		if (s->type == STRING_DATA || s->type == STRUCT_DATA) goto error;
			
		if (s->type == FLOAT_DATA) {
			val = s->fvalue;
		} else {
			val = s->lvalue;
		}
	}

	switch (cmd_ptr->token) {
		case FMIN:
		case FMAX:
			return (Math ());

		case ABS:
			if (s->type == FLOAT_DATA) {
				if (s->fvalue < 0) {
					s->fvalue = -(s->fvalue);
				}
			} else {
				if (s->lvalue < 0) {
					s->lvalue = -(s->lvalue);
				}
			}
			return (true);

		case SQRT:
			if (val < 0.0) val = 0.0;
			val = sqrt (val);
			break;

		case EXP:
			if (val < -708.0) {
				val = -708.0;
			} else if (val > 709.0) {
				val = 709.0;
			}
			val = exp (val);
			break;

		case LOG10:
			if (val <= 0.0) val = 0.0000001;
			val = log10 (val);
			break;

		case LOG:
			if (val <= 0.0) val = 0.0000001;
			val = log (val);
			break;

		case RANDOM:
			s = stack [++sindex];

			if (s == NULL) {
				if (!stack.Add ()) {
					exe->Error ("Stack Overflow (%d records)", stack.Num_Records ());
					return (false);
				}
				s = stack [sindex];
			}
			val = random.Probability ();
			break;

		case POWER:
			s = stack [--sindex];

			if (s->type == FLOAT_DATA) {
				val = pow (s->fvalue, val);
			} else if (s->type == INT_DATA) {
				val = pow ((double) s->lvalue, val);
			} else {
				goto error;
			}
			break;

		default:
			exe->Error ("Function Code %d was Unrecognized", cmd_ptr->token);
			return (false);
	}
	s->type = FLOAT_DATA;
	s->fvalue = val;
	return (true);

error:
	exe->Error ("Function \"%s\" does not support %s Data", 
		Token_Name (FUNCTION, cmd_ptr->token), Data_Type (s->type));
	return (false);
}
