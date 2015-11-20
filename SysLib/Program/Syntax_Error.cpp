//*************************************************** 
//	Syntax_Error.cpp - syntax error messages
//***************************************************

#include "User_Program.hpp"

//-----------------------------------------------------------
//	Syntax_Error
//-----------------------------------------------------------

bool User_Program::Syntax_Error (char *text)
{
	exe->Error ("%s Syntax Problem on Line Number %d", text, line_num);
	return (false);
}

