//********************************************************* 
//	Assign.cpp - Assign a Data Item
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Assign
//---------------------------------------------------------

bool User_Program::Assign (void)
{
	char *target_types [] = {"Structure", 
		"Integer Field", "Decimal Field", "String Field", 
		"Integer Variable", "Decimal Variable", "String Variable", 
		"Record Number"};

	switch (cmd_ptr->type) {

		case DATA:
			if (s->type != STRUCT_DATA) goto error;

			file [cmd_ptr->token]->Copy_Fields (file [s->lvalue]);
			out_flag [cmd_ptr->token] = 1;
			break;

		case INTEGER:
		case FLOAT:
			if (s->type == INT_DATA) {
				file [cmd_ptr->token]->Put_Field (cmd_ptr->value, s->lvalue);
			} else if (s->type == FLOAT_DATA) {
				file [cmd_ptr->token]->Put_Field (cmd_ptr->value, s->fvalue);
			} else {
				goto error;
			}
			out_flag [cmd_ptr->token] = 1;
			break;

		case STRING:
			if (s->type != STRING_DATA) goto error;

			file [cmd_ptr->token]->Put_Field (cmd_ptr->value, s->svalue);
			out_flag [cmd_ptr->token] = 1;
			break;
	
		case LVALUE:
			if (s->type == INT_DATA) {
				lvalue.Set (cmd_ptr->value, s->lvalue);
			} else if (s->type == FLOAT_DATA) {
				int value;

				if (s->fvalue > MAX_INTEGER) {
					value = MAX_INTEGER;
				} else if (s->fvalue < -MAX_INTEGER) {
					value = -MAX_INTEGER;
				} else {
					value = (int) s->fvalue;
				}
				lvalue.Set (cmd_ptr->value, value);
			} else {
				goto error;
			}
			break;
					
		case FVALUE:
			if (s->type == INT_DATA) {
				fvalue.Set (cmd_ptr->value, (double) s->lvalue);
			} else if (s->type == FLOAT_DATA) {
				fvalue.Set (cmd_ptr->value, s->fvalue);
			} else {
				goto error;
			}
			break;
				
		case SVALUE:
			if (s->type == STRING_DATA) {
				Svalue_Data *sptr = svalue [cmd_ptr->value];
				str_cpy (sptr->string, sptr->length, s->svalue);
			} else {
				goto error;
			}
			break;

		default:
			exe->Error ("Assignment to Type %d", cmd_ptr->type);
			return (false);
			break;
	}
	s = stack [sindex = 0];
	return (true);

error:
	exe->Error ("Illegal Assignment Statement\n\t\t%s = %s",
		target_types [cmd_ptr->type - DATA], Data_Type (s->type));
	return (false);
}
