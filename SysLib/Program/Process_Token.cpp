//********************************************************* 
//	Process_Token.cpp - process a token
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Process_Token
//---------------------------------------------------------

bool User_Program::Process_Token (void)
{
	switch (cmd.type) {
		
		case LIMIT:

			if (cmd.token == EQUALS) {
				if (!last_value) {
					return (Syntax_Error ("Delimiter"));
				}
			} else if (cmd.token == CLOSE_PAR) {
				if (!Process_Command (LIMIT)) return (false);
				last_value = 1;
				return (true);
			} else if (cmd.token == COMMA) {
				if (!Process_Command (LIMIT)) return (false);
				last_value = 0;
				return (true);
			} else if (cmd.token == END_CMD) {
				if (nest != -1) {
					return (Syntax_Error ("Logical Nesting"));
				}
				if (loop != -1) {
					return (Syntax_Error ("Loop Nesting Problem"));
				}
				if (!Process_Command (0)) return (false);
				level = assign_set = 0;
				return (true);
			}
			level++;
			last_value = 0;
			break;
	
		case LOGICAL:

			assign_set = 0;
			if (cmd.token == WHILE) {
				if (last_value > 0) {
					if (!Process_Command (LIMIT)) return (false);
				}
				if (++loop >= MAX_LOOP) {
					exe->Error ("Too Many Nested WHILE Commands");
					return (false);
				}
				loop_jump [loop] = command.Num_Records ();
				level++;
				last_value = 0;
			} else if (cmd.token == LOOP || cmd.token == ENDLOOP) {
				if (level < 2 || loop < 0) {
					return (Syntax_Error ("Loop"));
				}
				if (!Process_Command (LOGICAL)) return (false);

				last_value = -1;
				if (cmd.token == ENDLOOP) {
                    loop--;
					return (true);
				} else {
					level++;
				}
			} else if (cmd.token == BREAK) {
				if (last_value > 0) {
					if (!Process_Command (LIMIT)) return (false);
				}
				if (level < 2 || loop < 0) {
					return (Syntax_Error ("Loop"));
				}
				if (num_break [loop] >= MAX_BREAK) {
					exe->Error ("Too Many BREAK Statements for a WHILE Command");
				}
				if (!command.Add (&cmd)) goto mem_error;

				break_jump [loop] [num_break [loop]++] = command.Record_Index ();
				return (true);

			} else if (cmd.token == IF) {
				if (last_value > 0) {
					if (!Process_Command (LIMIT)) return (false);
				}
				if_level = ++level;
				if (++nest >= MAX_NEST) {
					exe->Error ("Too Many Nested IF Commands");
					return (false);
				}
				last_value = 0;
			} else {
				if (level < 2 || nest < 0) {
					return (Syntax_Error ("Logical"));
				} else if (cmd.token == THEN) {
					if (!Process_Command (LIMIT) || level != if_level) {
						return (Syntax_Error ("Logical"));
					}
				}
				if (!Process_Command (LOGICAL)) return (false);
				
				last_value = -1;
				
				if (cmd.token == ENDIF) {
					nest--;
					return (true);
				} else {
					level++;
				}
			}
			break;
		
		case RELATION:

			if (last_value > 0) {
				if (!Process_Command (RELATION)) return (false);
			} else if (cmd.token != NOT) {
				return (Syntax_Error ("Relational"));
			}
			level++;
			last_value = 0;
			break;
		
		case MATH:

			if (level < 2) {
				return (Syntax_Error ("Math"));
			}
			if (last_value > 0) {
				if (!Process_Command (MATH)) return (false);
			} else if (cmd.token == MINUS) {
				cmd.token = NEGATIVE;
			} else {
				return (Syntax_Error ("Math"));
			}
			level++;
			last_value = 0;				
			break;
		
		case FUNCTION:
		case CONVERT:
		case DATETIME:
		case IN_OUT:
		case TABLE:
		case RETURN:

			if (last_value > 0) {
				if (!Process_Command (LIMIT)) return (false);
			}
			level++;
			last_value = 0;
			break;

		case DATA:
		case INTEGER:
		case FLOAT:
		case STRING:
		case LVALUE:
		case FVALUE:
		case SVALUE:
		case RECORD:

			//---- write to a variable ----

			if (last_value) {
				if (last_value > 0) {
					if (!Process_Command (LIMIT)) return (false);
				}
				level++;
				last_value = -1;
				assign_set = cmd.token;
				
				if (cmd.type == RECORD) {
					return (Syntax_Error ("Assignment"));
				} else if (cmd.type == LVALUE || 
					cmd.type == FVALUE || 
					cmd.type == SVALUE) {

					if (cmd.token == 0) {
						return (Syntax_Error ("Constant"));
					}
				} else {
					file_stat [cmd.token] |= 2;

					if (read_only [cmd.token]) {
						return (Syntax_Error ("Read Only"));
					}
				}

			} else {

				//---- read a variable ----

				if (!command.Add (&cmd)) goto mem_error;

				last_value = 1;
				
				if (cmd.type == INTEGER || cmd.type == FLOAT ||
					cmd.type == STRING || cmd.type == DATA) {

					if (cmd.token == assign_set || !(file_stat [cmd.token] & 2)) {
						file_stat [cmd.token] |= 1;
						if (first_read < 0) {
							first_read = cmd.token;
						}
					}
				}
				return (true);
			}
			break;
	}
	
	//---- store the current command ----
	
	token_level.Set (level, &cmd);

	return (true);

mem_error:
	exe->Error ("Insufficient Memory for Token Processing");
	return (false);
}
