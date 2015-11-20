//********************************************************* 
//	Convert.cpp - Type Conversion Functions
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Convert
//---------------------------------------------------------

bool User_Program::Convert (void)
{
	//--- sub string function ----

	if (cmd_ptr->token == SUBSTR) {
		int i1, i2, len;
		char *str;

		if (sindex <= 2) goto string_error;

		Stack_Data *s2 = stack [sindex - 2];

		if (s2->type != STRING_DATA || s1->type != INT_DATA || 
			s->type != INT_DATA) goto string_error;
	
		i1 = s1->lvalue;
		i2 = s->lvalue;
		str = s2->svalue;
		len = (int) strlen (str);

		if (i1 < 0) i1 = 0;
		if (i2 < i1) i2 = i1;
		if (i2 > len) {
			i2 = len;
			if (i1 > i2) i1 = i2;
		}
		len = i2 - i1 + 1;
		if (len >= (int) sizeof (token)) goto string_error;

		memcpy (token, str + i1, len);		
		token [len] = '\0';

		sindex -= 2;
		s = s2;
		s->svalue = token;
		s->length = sizeof (token);
		return (true);
	}

	//---- type conversion functions ----

	if (s->type == STRUCT_DATA) goto error;

	if (cmd_ptr->token != ATOI && cmd_ptr->token != ATOF) {
		if (s->type == STRING_DATA) goto error;
	} else {
		if (s->type != STRING_DATA) goto error;
	}
							
	switch (cmd_ptr->token) {
	
		case FINT:
			if (s->type == FLOAT_DATA) {
				s->type = INT_DATA;
				s->lvalue = (int) s->fvalue;
			}
			break;
			
		case FFLOAT:
			if (s->type == INT_DATA) {
				s->type = FLOAT_DATA;
				s->fvalue = (double) s->lvalue;
			}
			break;
			
		case ROUND:
			if (s->type == FLOAT_DATA) {
				s->type = INT_DATA;
				s->lvalue = (int) (s->fvalue + 0.5);
			}
			break;	

		case ATOI:
			s->type = INT_DATA;
			s->lvalue = atol (s->svalue);
			break;
			
		case ATOF:
			s->type = FLOAT_DATA;
			s->fvalue = atof (s->svalue);
			break;
			
		case ITOA:
			if (s->type == FLOAT_DATA) {
				s->lvalue = (int) s->fvalue;
			}
			str_fmt (token, sizeof (token), "%d", s->lvalue);
			s->type = STRING_DATA;
			s->svalue = token;
			s->length = sizeof (token);
			break;
			
		case FTOA:
			if (s->type == INT_DATA) {
				s->fvalue = s->lvalue;
			}
			str_fmt (token, sizeof (token), "%lf", s->fvalue);
			s->type = STRING_DATA;
			s->svalue = token;
			s->length = sizeof (token);
			break;

		default:
			goto token_error;
			break;
	}
	return (true);

error:
	exe->Error ("Conversion \"%s\" does not support %s Data", 
		Token_Name (CONVERT, cmd_ptr->token), Data_Type (s->type));
	return (false);

token_error:
	exe->Error ("Conversion Token %d was Unrecognized", cmd_ptr->token);
	return (false);

string_error:
	exe->Error ("Illegal SUBSTR Function Syntax");
	return (false);
}
