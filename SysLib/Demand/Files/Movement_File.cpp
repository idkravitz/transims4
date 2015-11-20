//********************************************************* 
//	Movement_File.cpp - Turn Movement Input/Output
//*********************************************************

#include "Movement_File.hpp"

//-----------------------------------------------------------
//	Movement_File constructors
//-----------------------------------------------------------

Movement_File::Movement_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Movement_File::Movement_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Movement_File destructor
//-----------------------------------------------------------

Movement_File::~Movement_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Movement_File::Setup (void)
{
	File_Type ("Turn Movement File");
	File_ID ("Movement");

	node = in_link = out_link = start = end = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Movement_File::Create_Fields (void) 
{
	Add_Field ("NODE", INTEGER, 10);
	Add_Field ("IN_LINK", INTEGER, 10);
	Add_Field ("OUT_LINK", INTEGER, 10);
	Add_Field ("START", STRING, TIME_BUFFER);
	Add_Field ("END", STRING, TIME_BUFFER);
	Add_Field ("VOLUME", INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Movement_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE");
	in_link = Required_Field ("INLINK", "IN_LINK", "LINK_IN", "LINKIN");
	out_link = Required_Field ("OUTLINK", "OUT_LINK", "LINK_OUT", "LINKOUT");
	volume = Optional_Field ("VOLUME", "MOVEMENT", "TURN", "COUNT");

	if (!node || !in_link || !out_link || !volume) return (false);

	start = Optional_Field ("START_TIME", "STARTTIME", "START");
	end = Optional_Field ("END_TIME", "ENDTIME", "END");

	return (true);
}
