//*********************************************************
//	Table.cpp - Interpret a Table
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Table
//---------------------------------------------------------

bool User_Program::Table (void)
{
	int i, j, num, index;
	Svalue_Data *sptr;

	i = cmd_ptr->token;
	j = cmd_ptr->value;

	num = table [i] [j].Num_Records ();

	if (num == 0) goto int_error;
		
	if (s->type == FLOAT_DATA) {
		index = (int) s->fvalue;
	} else if (s->type == INT_DATA) {
		index = (int) s->lvalue;
	} else {
		goto index_error;
	}

	if (index < 1 || index > num) goto range_error;

	index = table [i] [j] [index];
	
	switch (cmd_ptr->token) {
		case ITAB:	//---- int table ----
			s->type = INT_DATA;
			s->lvalue = lvalue [index];
			break;
			
		case RTAB:	//---- float table ----
			s->type = FLOAT_DATA;
			s->fvalue = fvalue [index];
			break;
			
		case STAB:	//---- string table ----
			s->type = STRING_DATA;
			sptr = svalue [index];
			s->svalue = sptr->string;
			s->length = sptr->length;
			break;
	}
	return (true);

int_error:
	exe->Error ("%s Table %d has not been Initialized", Data_Type (i), j);
	return (false);

index_error:
	exe->Error ("Illegal Table Index = %s", Data_Type (s->type));
	return (false);

range_error:
	exe->Error ("%s Table %d Index %d is Out of Range (1-%d)", 
		Data_Type (i), j, index, num);
	return (false);
}
