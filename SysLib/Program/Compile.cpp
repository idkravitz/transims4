//*********************************************************
//	Compile.cpp - Compile the User Program
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Compile
//---------------------------------------------------------

bool User_Program::Compile (char *command)
{
	bool slash, comment, quote;

	line_num++;

	while (*command) {
		
		buf = buffer;
		
		slash = false;
		comment = false;
		quote = false;
	
		//---- remove white space from the command ----
	
		while (*command) {
			if (*command == '\n' || *command == '\r' || *command == '\f') {
				while (*command && *command < 32) {
					command++;
				}
				if (*command) line_num++;
				break;
			}
			if (*command >= 32 && *command <= 126) {
				if (quote) {
					*buf = *command;
					
					if (slash) {
						if (*buf == '\\') {
							buf--;
							slash = false;
						} else if (*buf == 'f') {
							*(--buf) = '\f';
						} else if (*buf == 'n') {
							*(--buf) = '\n';
						} else if (*buf == 't') {
							*(--buf) = '\t';
						} else if (*buf == 'r') {
							*(--buf) = '\r';
						} else if (*buf == 'b') {
							*(--buf) = '\b';
						} else if (*buf == 'a') {
							*(--buf) = '\a';
						}
						slash = !slash;
					}
				} else {
					if ((*command == '#' && 
						(*(command+1) == '#' || *(command+1) == '-' || *(command+1) == '*')) ||
						(*command == '/' && 
						(*(command+1) == '/' || *(command+1) == '-' || *(command+1) == '*')) ||
						(*command == ';' && 
						(*(command+1) == ';' || *(command+1) == '-' || *(command+1) == '*'))) comment = true;

					if (!comment) {
						if (*command == ';') {
							command++;
							continue;
						}
						*buf = *command;
					}
				}
				if (!comment) {
					if (*buf == '"') quote = !quote;
					if (quote && *buf == '\\') slash = !slash;
					buf++;
				}
			}
			command++;
		}
		*buf = '\0';
		buf = buffer;

		if (declare_flag) {
			if (!Initial_Declare ()) break;
		} else if (table_flag) {
			if (!Initial_Table ()) break;
		} else {
			while (Get_Token ()) {
				if (!Process_Token ()) break;
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Compile from a file
//---------------------------------------------------------

bool User_Program::Compile (Db_File *script, bool list_flag)
{
	char *str_ptr;

	if (script == NULL) {
		exe->Error ("User Program Script File is NULL");
		return (false);
	}
	while (script->Read ()) {
		str_ptr = script->Record ();

		if (list_flag) {
			List_Commands (str_ptr);
		}
		if (!Compile (str_ptr)) {
			exe->Error ("Compiling User Program Script");
			return (false);
		}
	}
	Compile ("END");
	script->Close ();
	return (true);
}
