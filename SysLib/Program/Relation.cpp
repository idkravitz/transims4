//*********************************************************
//	Relation.cpp - logical relationships
//*********************************************************

#include "User_Program.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Relation
//---------------------------------------------------------

bool User_Program::Relation (void)
{
	int l1, l2;
	double f1, f2;
	char *str1, *str2, num [20];

	if (s->type == STRUCT_DATA || s1->type == STRUCT_DATA) goto error;

	//---- process a NOT statement ---

	if (cmd_ptr->token == NOT) {
		if (s->type == INT_DATA) {
			s->lvalue = !(s->lvalue);
		} else if (s->type == FLOAT_DATA) {
			s->fvalue = !(s->fvalue);
		} else {
			goto error;
		}
		return (true);
	}

	//---- check for string relationships ----

	if (s1->type == STRING_DATA || s->type == STRING_DATA) {
		if (s1->type == STRING_DATA) {
			str1 = s1->svalue;
		} else if (s1->type == FLOAT_DATA) {
			str1 = str_fmt (num, sizeof (num), "%.2lf", s1->fvalue);
		} else {
			str1 = str_fmt (num, sizeof (num), "%d", s1->lvalue);
		}
		if (s->type == STRING_DATA) {
			str2 = s->svalue;
		} else if (s->type == FLOAT_DATA) {
			str2 = str_fmt (num, sizeof (num), "%.2lf", s->fvalue);
		} else {
			str2 = str_fmt (num, sizeof (num), "%d", s->lvalue);
		}
		l1 = str_cmp (str1, str2);
		
		switch (cmd_ptr->token) {
			case EQ:
				l1 = (l1 == 0);
				break;
			case NEQ:
				l1 = (l1 != 0);
				break;
			case LT:
				l1 = (l1 < 0);
				break;
			case LE:
				l1 = (l1 <= 0);
				break;
			case GT:
				l1 = (l1 > 0);
				break;
			case GE:
				l1 = (l1 >= 0);
				break;
			case AND:
				l1 = (l1 == 0);
				break;
			case OR:
				l1 = (l1 != 0);
				break;
			default:
				goto token_error;
				break;
		}
		
	} else if (s1->type == FLOAT_DATA || s->type == FLOAT_DATA) {

		//---- process floating point relationships ----

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
			case EQ:
				l1 = (f1 == f2);
				break;
			case NEQ:
				l1 = (f1 != f2);
				break;
			case LT:
				l1 = (f1 < f2);
				break;
			case LE:
				l1 = (f1 <= f2);
				break;
			case GT:
				l1 = (f1 > f2);
				break;
			case GE:
				l1 = (f1 >= f2);
				break;
			case AND:
				l1 = (f1 && f2);
				break;
			case OR:
				l1 = (f1 || f2);
				break;
			default:
				goto token_error;
				break;
		}

	} else {

		//---- process integer relationships ----

		l1 = s1->lvalue;
		l2 = s->lvalue;

		switch (cmd_ptr->token) {
			case EQ:
				l1 = (l1 == l2);
				break;
			case NEQ:
				l1 = (l1 != l2);
				break;
			case LT:
				l1 = (l1 < l2);
				break;
			case LE:
				l1 = (l1 <= l2);
				break;
			case GT:
				l1 = (l1 > l2);
				break;
			case GE:
				l1 = (l1 >= l2);
				break;
			case AND:
				l1 = (l1 && l2);
				break;
			case OR:
				l1 = (l1 || l2);
				break;
			default:
				goto token_error;
				break;
		}
	}
	s = stack [--sindex];

	s->type = INT_DATA;
	s->lvalue = l1;
	return (true);

error:
	exe->Error ("Relationship \"%s\" does not support %s Data", 
		Token_Name (RELATION, cmd_ptr->token), Data_Type (s->type));
	return (false);

token_error:
	exe->Error ("Unrecognized Relationship Token = %d", cmd_ptr->token);
	return (false);
}
