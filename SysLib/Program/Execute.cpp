///************************************************** 
//	Execute.cpp - Execute a User Program
///**************************************************

#include "User_Program.hpp"

//-----------------------------------------------------------
//	Execute
//-----------------------------------------------------------

int	User_Program::Execute (int _record)
{
	int stat;
	bool test;

	memset (out_flag, '\0', num_files * sizeof (int));

	stat = 0;
	s = stack [sindex = 1];
	loop = 0;

	if (_record < 0) {
		record++;
	} else {
		record = _record;
	}

	//---- process each command ----

	for (cmd_ptr = command.First (); cmd_ptr; cmd_ptr = command.Next ()) {
		s1 = stack [sindex - 1];

		switch (cmd_ptr->type) {

			case LIMIT:
				if (cmd_ptr->token != EQUALS) {
					exe->Error ("Unrecognized Limit Token = %d (stack=%d)", 
						cmd_ptr->token, command.Record_Index ());
					return (-1);
				}
				cmd_ptr = command.Next ();

				if (!Assign ()) return (-1);
				break;

			case LOGICAL:
				if (cmd_ptr->token == IF || cmd_ptr->token == WHILE) {
					if (s->type == INT_DATA) {
						test = (s->lvalue != 0);
					} else if (s->type == FLOAT_DATA) {
						test = (s->fvalue != 0.0);
					} else if (s->type == STRING_DATA) {
						test = (s->svalue [0] != '\0');
					} else {
						test = false;
					}
					s = stack [sindex = 1];
					if (test) continue;
				}
				cmd_ptr = command [cmd_ptr->value];
				break;

			case RELATION:
				if (!Relation ()) return (-1);
				break;

			case MATH:
				if (!Math ()) return (-1);
				break;

			case FUNCTION:
				if (!Function ()) return (-1);
				break;

			case CONVERT:
				if (!Convert ()) return (-1);
				break;

			case DATETIME:
				if (!Date_Time ()) return (-1);
				break;

			case IN_OUT:
				if (!Input_Output ()) return (-1);
				break;

			case TABLE:
				if (!Table ()) return (-1);
				break;

			case RETURN:
				if (s->type == INT_DATA) {
					stat = s->lvalue;
				} else if (s->type == FLOAT_DATA) {
					stat = (int) s->fvalue;
				}
				return (stat);
				break;

			default:
				if (!Read_Value ()) return (-1);
				break;
		}
	}
	return (stat);
}
