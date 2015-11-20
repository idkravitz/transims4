//********************************************************* 
//	Process_Command.cpp - Process a Token Command
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Process_Command
//---------------------------------------------------------

bool User_Program::Process_Command (int type)
{
	static int math_level [] = {
		0, 0, 1, 1, 2, 3, 4
	};

	static int rel_level [] = {
		2, 2, 2, 2, 2, 2, 1, 0, 0
	};
	
	int i, jump;
	
	while (level > 1) {
		cmd_ptr = token_level [level];

		if (type == MATH) {
	
			if (cmd_ptr->type != MATH) return (true);
		
			if (math_level [cmd_ptr->token - PLUS] <
				math_level [cmd.token - PLUS]) return (true);
				
		} else if (type == RELATION) {
		
			if (cmd_ptr->type != RELATION) return (true);

			if (rel_level [cmd_ptr->token - EQ] < rel_level [cmd.token - EQ]) return (true);
				
		} else if (type == LIMIT && cmd_ptr->token == OPEN_PAR) {
			
			if (cmd.token == CLOSE_PAR) {
				cmd_ptr = token_level [--level];
				
				if (cmd_ptr->type == FUNCTION || cmd_ptr->type == CONVERT ||
					cmd_ptr->type == DATETIME || cmd_ptr->type == IN_OUT ||	
					cmd_ptr->type == TABLE || cmd_ptr->type == RETURN) {

					cmd_ptr = token_level [level--];

					if (!command.Add (cmd_ptr)) goto mem_error;
				}
			}
			return (true);

		} else if (type == LIMIT && cmd_ptr->token == IF) {

			if (cmd.token == THEN) {
				if (level == if_level + 1) level--;
				return (true);
			}
			return (Syntax_Error ("Logical"));
		}
		
		if (cmd_ptr->token == EQUALS) {

			if (!command.Add (cmd_ptr)) goto mem_error;

			cmd_ptr = token_level [--level];

			if (!command.Add (cmd_ptr)) goto mem_error;
			
			level--;
			if (type == LIMIT) return (true);
			
		} else if (cmd_ptr->type == LOGICAL) {
		
			if (type != LOGICAL) return (true);
			level--;

			if (cmd_ptr->token == WHILE) {
				if (cmd.token != LOOP) {
					return (Syntax_Error ("Loop"));
				}
				if (!command.Add (cmd_ptr)) goto mem_error;

				while_jump [loop] = command.Record_Index ();
				num_break [loop] = 0;

			} else if (cmd_ptr->token == LOOP) {
				if (cmd.token == ENDLOOP) {

					//---- add the loop jump ----

					cmd_ptr->value = loop_jump [loop];

					if (!command.Add (cmd_ptr)) goto mem_error;

					//---- add the break jumps ----

					jump = command.Record_Index ();

					for (i=0; i < num_break [loop]; i++) {
						cmd_ptr = command [break_jump [loop] [i]];

						if (cmd_ptr != NULL) {
							cmd_ptr->value = jump;
						}
					}

					//---- add the while jump ----
					
					cmd_ptr = command [while_jump [loop]];

					if (cmd_ptr != NULL) {
						cmd_ptr->value = jump;
					}

				} else {
					return (Syntax_Error ("Loop"));
				}
			} else if (cmd_ptr->token == IF) {
				if (cmd.token != THEN) {
					return (Syntax_Error ("Logical"));
				}
				if (!command.Add (cmd_ptr)) goto mem_error;

				if_jump [nest] = command.Record_Index ();

			} else if (cmd_ptr->token == THEN) {
			
				if (cmd.token == ELSE || cmd.token == ENDIF) {
					if (cmd.token == ELSE) {
						if (!command.Add (cmd_ptr)) goto mem_error;

						then_jump [nest] = jump = command.Record_Index ();
					} else {
						jump = command.Num_Records ();
					}
					cmd_ptr = command [if_jump [nest]];

					if (cmd_ptr != NULL) {
						cmd_ptr->value = jump;
					}

				} else {
					return (Syntax_Error ("Logical"));
				}
			} else if (cmd_ptr->token == ELSE) {
				
				if (cmd.token != ENDIF) {
					return (Syntax_Error ("Logical"));
				}
				jump = command.Num_Records ();
				
				cmd_ptr = command [then_jump [nest]];

				if (cmd_ptr != NULL) {
					cmd_ptr->value = jump;
				}				
			}
			return (true);
			
		} else {
			if (!command.Add (cmd_ptr)) goto mem_error;
			level--;	
		}
	}
	return (true);

mem_error:
	exe->Error ("Insufficient Memory for Token Processing");
	return (false);
}
