//********************************************************* 
//	List_Commands.cpp - Lists Commands to the Printer
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	List_Commands
//---------------------------------------------------------

void User_Program::List_Commands (char *command)
{
	while (*command) {
		buf = buffer;
	
		while (*command) {
			if (*command == '\n' || *command == '\r' || *command == '\f') {
				*buf = '\0';
				while (*command && *command < 32 && *command != '\t') {
					command++;
				}
				break;
			}
			if ((*command >= 32 && *command <= 126) || *command == '\t') {
				*buf++ = *command;
			}
			command++;
		}
		*buf = '\0';

		exe->Print (1, "\t%s", buffer);
	}
}
