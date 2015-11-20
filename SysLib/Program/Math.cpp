//********************************************************* 
//	Math.cpp - Math Operations
//*********************************************************

#include "User_Program.hpp"

#include <math.h>

//---------------------------------------------------------
//	Math
//---------------------------------------------------------

bool User_Program::Math (void)
{
	int l1, l2;
	double f1, f2;

	char *math_types [] = {"Plus", "Minus",	"Multiply", "Divide", "Modulo", "Power", "Negative"};

	if (cmd_ptr->token == NEGATIVE) {

		if (s->type == STRUCT_DATA || s->type == STRING_DATA) goto error;

		//---- reverse the sign ----
						
		if (s->type == INT_DATA) {
			s->lvalue = -(s->lvalue);
		} else if (s->type == FLOAT_DATA) {
			s->fvalue = -(s->fvalue);
		} else {
			goto error;
		}
		return (true);
	}
	
	if (s->type == STRUCT_DATA || s1->type == STRUCT_DATA) goto error;

	//---- process the data types ----

	if (s->type == STRING_DATA || s1->type == STRING_DATA) {

		//---- string contatination ----

		if (cmd_ptr->token != PLUS || 
			s->type != STRING_DATA || 
			s1->type != STRING_DATA) goto error;

		str_cat (s1->svalue, s1->length, s->svalue);

	} else if (s1->type == FLOAT_DATA || s->type == FLOAT_DATA) {

		//---- floating point math ----

		if (s1->type == FLOAT_DATA) {
			f1 = s1->fvalue;
		} else {
			f1 = s1->lvalue;
		}
		if (s->type == FLOAT_DATA) {
			f2 = s->fvalue;
		} else {
			f2 = s->lvalue;
		}
		
		switch (cmd_ptr->token) {
			case PLUS:
				f1 += f2;
				break;
			case MINUS:
				f1 -= f2;
				break;
			case MULTIPLY:
				f1 *= f2;
				break;
			case DIVIDE:
				if (f2) {
					f1 /= f2;
				}
				break;
			case MODULO:
				if (f2) {
					f1 = (f1 / f2) - 1.0;
				}
				break;
			case POWER:
				f1 = pow (f1, f2);
				break;
			case FMIN:
				if (f2 < f1) f1 = f2;
				break;
			case FMAX:
				if (f2 > f1) f1 = f2;
				break;
			default:
				goto token_error;
				break;

		}
		s1->type = FLOAT_DATA;
		s1->fvalue = f1;
		
	} else {

		l1 = s1->lvalue;
		l2 = s->lvalue;
		
		switch (cmd_ptr->token) {
			case PLUS:
				l1 += l2;
				break;
			case MINUS:
				l1 -= l2;
				break;
			case MULTIPLY:
				l1 *= l2;
				break;
			case DIVIDE:
				if (l2) {
					l1 /= l2;
				}
				break;
			case MODULO:
				if (l2) {
					l1 %= l2;
				}
				break;
			case POWER:
				l1 = (int) pow ((double) l1, (double) l2);
				break;
			case FMIN:
				if (l2 < l1) l1 = l2;
				break;
			case FMAX:
				if (l2 > l1) l1 = l2;
				break;
			default:
				goto token_error;
				break;
		}
		s1->type = INT_DATA;
		s1->lvalue = l1;
	}
	s = stack [--sindex];
	return (true);

error:
	if (cmd_ptr->token == FMIN || cmd_ptr->token == FMAX) {
		exe->Error ("Function \"%s\" does not support %s Data", 
			Token_Name (FUNCTION, cmd_ptr->token), Data_Type (s->type));
	} else if (cmd_ptr->token == NEGATIVE) {
		exe->Error ("%s Operator does not support %s Data",  
			math_types [cmd_ptr->token - PLUS], Data_Type (s->type));
	} else {
		exe->Error ("%s Operator does not support %s and %s Data",  
			math_types [cmd_ptr->token - PLUS], Data_Type (s->type), Data_Type (s1->type));
	}
	return (false);

token_error:
	exe->Error ("Unrecognized Math Token = %d", cmd_ptr->token);
	return (false);
}
