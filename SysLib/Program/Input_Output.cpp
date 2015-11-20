//*************************************************** 
//	Input_Output.cpp - input-output functions
//***************************************************

#include "User_Program.hpp"

//-----------------------------------------------------------
//	Input_Output
//-----------------------------------------------------------

bool User_Program::Input_Output (void)
{
	Stack_Data *s2, *s3;
	int fh, len, offset, max;
	char *record, format [STRING_BUFFER], *ptr;

	if (s->type == STRUCT_DATA) goto error;

	if (cmd_ptr->token == INPUT || cmd_ptr->token == OUTPUT) {

		if (s->type != INT_DATA || s1->type != STRUCT_DATA) goto error;

        fh = s1->lvalue;
		record = file [fh]->Record ();
		offset = s->lvalue;

		if (cmd_ptr->token == INPUT) {
			//s1->lvalue = file [fh]->Read (record, size, offset);
		} else {
			//s1->lvalue = file [fh]->Write (record, &offset);
		}
		s1->type = INT_DATA;

	} else {

		if (s1->type != STRING_DATA) goto error;

		if (cmd_ptr->token == PRINT || cmd_ptr->token == LIST || cmd_ptr->token == FORMAT) {

			//----- check the format syntax ----

			str_cpy (buffer, sizeof (buffer), s1->svalue);

			ptr = strchr (buffer, '%');

			while (ptr != NULL) {
				++ptr;
				if (*ptr != '-' && *ptr != '.' && (*ptr < '0' || *ptr > '9')) {
					str_cpy (format, sizeof (format), buffer, (ptr - buffer));
					break;
				}
			}

			//---- format the data ---

			switch (s->type) {
				case INT_DATA:
					if (ptr != NULL && *ptr != 'd') {
						str_cat (format, sizeof (format), "d");
						str_cat (format, sizeof (format), ptr);
						str_fmt (buffer, sizeof (buffer), format, s->lvalue);
					} else {
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->lvalue);
					}
					break;
				case FLOAT_DATA:
					if (ptr != NULL && *ptr != 'l' && *ptr != 'f' && *ptr != 'g') {
						str_cat (format, sizeof (format), "lf");
						str_cat (format, sizeof (format), ptr);
						str_fmt (buffer, sizeof (buffer), format, s->fvalue);
					} else {
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->fvalue);
					}
					break;
				case STRING_DATA:
					if (ptr != NULL && *ptr != 's') {
						str_cat (format, sizeof (format), "s");
						str_cat (format, sizeof (format), ptr);
						str_fmt (buffer, sizeof (buffer), format, s->svalue);
					} else {
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->svalue);
					}
					break;
				default:
					str_cpy (buffer, sizeof (buffer), s1->svalue);
					break;
			}
			if (cmd_ptr->token == FORMAT) {
				s1->svalue = buffer;
			} else {
				s = stack [--sindex];

                if (cmd_ptr->token == PRINT) {
					exe->Print (1, buffer);
				} else {
					exe->Show_Message (buffer);
				}
			}

		} else if (cmd_ptr->token == FREAD || cmd_ptr->token == FWRITE) {

			s2 = stack [sindex - 2];
			s3 = stack [sindex - 3];

			if (s3->type != STRUCT_DATA || s2->type != INT_DATA) goto error;

			fh = s3->lvalue;
			len = file [fh]->Record_Size ();
			record = file [fh]->Record ();
			offset = s2->lvalue;

			if (cmd_ptr->token == FREAD) {

				//---- read ascii data ----

				max = FIELD_BUFFER;
				//max = Max_Field_Width ();
				if (offset + max > len) goto record_error;
				
				//str_ncpy (token, sizeof (token), record + offset, max);

				token [max] = '\0';

				switch (s->type) {
					case INT_DATA:
						//if (sscanf (token, buffer, &(s->lvalue)) != 1) s->lvalue = 0;
						break;
					case FLOAT_DATA:
						//if (sscanf (token, buffer, &(s->fvalue)) != 1) s->fvalue = 0.0;
						break;
					case STRING_DATA:
						//if (sscanf (token, buffer, &(s->svalue)) != 1) s->svalue = token;
						break;
				}
				*s3 = *s;

			} else {

				//---- write ascii data ----

				switch (s->type) {
					case INT_DATA:
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->lvalue);
						break;
					case FLOAT_DATA:
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->fvalue);
						break;
					case STRING_DATA:
						str_fmt (buffer, sizeof (buffer), s1->svalue, s->svalue);
						break;
				}
				max = (int) strlen (buffer);

				if (offset + max > len) goto record_error;

				memcpy (record + offset, token, strlen (token));
				out_flag [fh] = 1;
					
				s3->type = INT_DATA;
				s3->lvalue = offset + max;
			}
			sindex -= 2;
			s = s2;

		} else {
			exe->Error ("Input-Output Token %d was Unrecognized", cmd_ptr->token);
			return (false);
		}
	}
	s = stack [--sindex];
	return (true);

record_error:
	exe->Error ("ASCII Record Offset %d-%d is Out-of-Range (0-%d)", offset, (offset + max), len);
	return (false);

error:
	exe->Error ("Input-Output Function \"%s\" does not support %s Data", 
		Token_Name (IN_OUT, cmd_ptr->token), Data_Type (s->type));
	return (false);
}

