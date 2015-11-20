//********************************************************* 
//	Read_Value.cpp - read variables from data structures
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Read_Value
//---------------------------------------------------------

bool User_Program::Read_Value (void)
{
	int code;
	Svalue_Data *sptr;

	s = stack [++sindex];

	if (s == NULL) {
		if (!stack.Add ()) {
			exe->Error ("Stack Overflow (%d > %d)", sindex, stack.Num_Records ());
			return (false);
		}
		s = stack [sindex];
	}
			
	switch (cmd_ptr->type) {
		case DATA:
			s->type = STRUCT_DATA;
			s->lvalue = cmd_ptr->token;
			break;
				
		case INTEGER:
			s->type = INT_DATA;
			file [cmd_ptr->token]->Get_Field (cmd_ptr->value, &(s->lvalue));
			break;
				
		case FLOAT:
			s->type = FLOAT_DATA;
			file [cmd_ptr->token]->Get_Field (cmd_ptr->value, &(s->fvalue));
			break;
				
		case STRING:
			s->type = STRING_DATA;
			code = cmd_ptr->value * num_files + cmd_ptr->token;

			Svalue_Index *sindex;

			sindex = svalue_index.Get (code);

			if (sindex == NULL) {
				exe->Error ("Svalue Index for Code %d Not Found", code);
			}
			sptr = svalue [sindex->index];

			s->length = sptr->length;
			s->svalue = sptr->string;

			file [cmd_ptr->token]->Get_Field (cmd_ptr->value, s->svalue);
			break;
			
		case LVALUE:
			s->type = INT_DATA;
			s->lvalue = lvalue [cmd_ptr->value];
			break;
			
		case FVALUE:
			s->type = FLOAT_DATA;
			s->fvalue = fvalue [cmd_ptr->value];
			break;
				
		case SVALUE:
			s->type = STRING_DATA;
			sptr = svalue [cmd_ptr->value];

			s->length = sptr->length;
			s->svalue = sptr->string;
			break;

		case RECORD:
			s->type = INT_DATA;
			s->lvalue = record;
			break;

		default:
			exe->Error ("Token Type %d was Not Recognized", cmd_ptr->type);
			return (false);
	}
	return (true);
}
