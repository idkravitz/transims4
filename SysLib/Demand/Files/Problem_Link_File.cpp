//********************************************************* 
//	Problem_Link_File.cpp - Problem Link File Input/Output
//*********************************************************

#include "Problem_Link_File.hpp"

//-----------------------------------------------------------
//	Problem_Link_File constructors
//-----------------------------------------------------------

Problem_Link_File::Problem_Link_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Problem_Link_File::Problem_Link_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Problem_Link_File destructor
//-----------------------------------------------------------

Problem_Link_File::~Problem_Link_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Problem_Link_File::Setup (void)
{
	File_Type ("Problem Link File");
	File_ID ("ProblemLink");

	link = node = start = end = problem = count = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Problem_Link_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("START_TIME", STRING, TIME_BUFFER);
		Add_Field ("END_TIME", STRING, TIME_BUFFER);
		Add_Field ("PROBLEM", INTEGER, 2);
		Add_Field ("COUNT", INTEGER, 10);
	} else {
		Add_Field ("LINK", INTEGER, 10);
		Add_Field ("DIR", INTEGER, 1);
		Add_Field ("START_TIME", STRING, TIME_BUFFER);
		Add_Field ("END_TIME", STRING, TIME_BUFFER);
		Add_Field ("PROBLEM", INTEGER, 2);
		Add_Field ("COUNT", INTEGER, 10);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Problem_Link_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	node = Required_Field ("DIR", "NODE");
	problem = Required_Field ("PROBLEM");
	count = Required_Field ("COUNT");

	if (!link || !node || !problem || !count) return (false);

	//---- optional fields ----

	start = Optional_Field ("START_TIME", "STARTTIME", "START");
	end = Optional_Field ("END_TIME", "ENDTIME", "END");

	return (true);
}
