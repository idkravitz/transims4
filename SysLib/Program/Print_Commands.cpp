//********************************************************* 
//	Print_Commands.cpp - Write Commands to the Printer
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Print_Commands
//---------------------------------------------------------

void User_Program::Print_Commands (bool header)
{
	int i;
	char *str;
	
	char *type_text [] = {
		"Error",
		"Assign",
		"Logical",
		"Relation",
		"Math",
		"Function",
		"Convert",
		"Date-Time",
		"In/Output",
		"Table",
		"Return",
		"Declare",
		"Structure",
		"Integer",
		"Real",
		"String",
		"Integer",
		"Real",
		"String",
		"Record"
	};

	//---- print the page header ----

	if (!exe->Break_Check (command.Num_Records () + 5)) {
		exe->Print (1);
		exe->Page_Header ();
	}
	if (header) {
		exe->Print (1, "Command Processing Stack");
		exe->Print (1);
	}

	//---- process the command stack ----

	i = 0;
	
	for (cmd_ptr = command.First (); cmd_ptr != NULL; cmd_ptr = command.Next ()) {

		i = command.Record_Index ();

		buffer [0] = '\0';

		switch (cmd_ptr->type) {
			case LOGICAL:
				if (cmd_ptr->token == IF || cmd_ptr->token == WHILE) {
					str_fmt (buffer, sizeof (buffer), "If False, Jump to %d", cmd_ptr->value + 1);
				} else {
					str_fmt (buffer, sizeof (buffer), "Jump to %d", cmd_ptr->value + 1);
				}
				break;

			case RELATION:
			case FUNCTION:
			case CONVERT:
			case DATETIME:
			case IN_OUT:
				str_cpy (buffer, sizeof (buffer), Token_Name (cmd_ptr->type, cmd_ptr->token));
				break;

			case MATH:
			case LIMIT:
				if (cmd_ptr->token == NEGATIVE) {
					str_cpy (buffer, sizeof (buffer), "Negative");
				} else {
					str_cpy (buffer, sizeof (buffer), Token_Name (cmd_ptr->type, cmd_ptr->token));
				}
				break;

			case TABLE:
				if (cmd_ptr->token == ITAB) {
					str_fmt (buffer, sizeof (buffer), "ITAB%d", cmd_ptr->value);
				} else if (cmd_ptr->type == RTAB) {
					str_fmt (buffer, sizeof (buffer), "RTAB%d", cmd_ptr->value);
				} else {
					str_fmt (buffer, sizeof (buffer), "STAB%d", cmd_ptr->value);
				}
				break;

			case RETURN:
				str_cpy (buffer, sizeof (buffer), "Integer");
				break;

			case DATA:
				str_cpy (buffer, sizeof (buffer), file [cmd_ptr->token]->File_ID ());
				break;

			case INTEGER:
			case FLOAT:
			case STRING:
				str_cpy (buffer, sizeof (buffer), file [cmd_ptr->token]->File_ID ());
				str_cat (buffer, sizeof (buffer), ".");
				str_cat (buffer, sizeof (buffer), file [cmd_ptr->token]->Field (cmd_ptr->value)->Name ());
				break;

			case RECORD:
                str_cpy (buffer, sizeof (buffer), "Integer");
				break;

			case LVALUE:
			case FVALUE:
			case SVALUE:
				if (cmd_ptr->token == 0) {	
					
					//---- constant ----

					if (cmd_ptr->type == LVALUE) {
						str_fmt (buffer, sizeof (buffer), "%d", lvalue [cmd_ptr->value]);
					} else if (cmd_ptr->type == FVALUE) {
						str_fmt (buffer, sizeof (buffer), "%lf", fvalue [cmd_ptr->value]);
					} else {
						buf = buffer;
						str = svalue [cmd_ptr->value]->string;

						*buf++ = '"';

						while (*str != '\0') {
							if (*str == '\n') {
								*buf++ = '\\';
								*buf++ = 'n';
							} else if (*str == '\t') {
								*buf++ = '\\';
								*buf++ = 't';
							} else {
								*buf++ = *str;
							}
							str++;
						}
						*buf++ = '"';
						*buf = '\0';
					}

				} else {

					//---- variable ----

					str = Variable_Name (cmd_ptr->type, cmd_ptr->value);

					if (str == NULL) {
						exe->Error ("type=%d, value=%d", cmd_ptr->type, cmd_ptr->value);
					}

					str_cpy (buffer, sizeof (buffer), str);

				}
				break;

			default:
				str_fmt (buffer, sizeof (buffer), "??? %d", cmd_ptr->token);
				break;
	
		}

		//---- print the line ----

		if (header && exe->Break_Check (1)) {
			exe->Print (1, "Command Processing Stack");
			exe->Print (1);
		}
		exe->Print (1, "%3d) %-10.10s   %s", i, type_text [cmd_ptr->type], buffer);	
	}
	exe->Print (1, "%3d) End", i + 1);
	exe->Print (1);
}	
	 
/**********************************************************|***********************************************************

	Command Processing Stack

	xxx) ssssssssss   sssssssssssssssssssssss

***********************************************************|***********************************************************/ 
