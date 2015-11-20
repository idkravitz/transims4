//*********************************************************
//	Initial_Declare.cpp - interpret a list of variables
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Initial_Declare
//---------------------------------------------------------

bool User_Program::Initial_Declare (void)
{
	Token_Data var;
	Svalue_Data svalue_rec;

	char *ptr;
	int i, j, len;
	bool range;

	len = 0;
	range = false;
	var.name = NULL;
	var.code = 0;
	var.type = 0;

	for (;;) {
	
		while (*buf == ' ' || *buf == '\t') {
			buf++;
		}
		if (*buf == '\0') break;
	
		ptr = token;
	
		//---- determine the token type ----

		if ((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z')) {
				
			do {
				*ptr++ = *buf++;
			} while ((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z') ||
				(*buf >= '0' && *buf <= '9') || *buf == '_');
			
			*ptr = '\0';
			
			if (!(str_cmp ("ENDDEF", token))) {
				declare_flag = false;
				return (false);
			}

			//---- look for the variable in the list ----

			if (Variable (token) != NULL) {
				exe->Error ("User Variable \"%s\" was Redefined", token);
				return (false);
			}

			//---- process a variable range ----

			i = j = 0;
			len = (int) strlen (token) + 1;

			if (range) {
				ptr = token;

				for (; i < len; i++, ptr++) {
					if (*ptr == '\0' || var.name [i] == '\0') break;

					if ((*ptr >= 'A' && *ptr <= 'Z') || 
						(*ptr >= 'a' && *ptr <= 'z') || *ptr == '_') {
						if (*ptr != var.name [i]) goto range_error;
					} else if (var.name [i] >= '0' && var.name [i] <= '9') {
						i = atoi (var.name + i);
						j = atoi (ptr);
						break;
					} else {
						goto range_error;
					}
				}
				if (i >= j) goto range_error;
				*ptr = '\0';
				i++;
			}

			//---- create new variables ----

			for (; i <= j; i++) {

				var.name = new char [len];

				if (var.name == NULL) goto mem_error;

				if (range) {
					str_fmt (var.name, len, "%s%d", token, i);
				} else {
					str_cpy (var.name, len, token);
				}

				switch (declare_type) {
					case IVAR:
						if (!lvalue.Add (0)) goto mem_error;
						var.type = LVALUE;
						var.code = lvalue.Record_Index ();
						break;
					case RVAR:
						if (!fvalue.Add (0.0)) goto mem_error;
						var.type = FVALUE;
						var.code = fvalue.Record_Index ();
						break;
					case SVAR:
						svalue_rec.string = new char [STRING_BUFFER];
						if (svalue_rec.string == NULL) goto mem_error;
						svalue_rec.string [0] = '\0';
						svalue_rec.length = STRING_BUFFER;

						if (!svalue.Add (&svalue_rec)) goto mem_error;
						var.type = SVALUE;
						var.code = svalue.Record_Index ();
						break;
				}
				if (!Add_Variable (&var)) goto mem_error;
			}
			range = false;
			
		} else if (*buf == ',') {

			buf++;

		} else if (*buf == '.' && *(buf+1) == '.') {

			if (len == 0) goto range_error;

			range = true;
			
			while (*buf == '.') buf++;

		} else {

			exe->Error ("Declaring User Variables");
			return (false);
		}
	}
	return (true);

mem_error:
	exe->Error ("Insufficient Memory for %s Variable %s", Data_Type (declare_type), token);
	return (false);

range_error:
	exe->Error ("Illegal Declaration Range at %s Variable %s", Data_Type (declare_type), token);
	return (false);
}
