//********************************************************* 
//	System_Event_File.cpp - System Event Input/Output
//*********************************************************

#include "System_Event_File.hpp"

//-----------------------------------------------------------
//	System_Event_File constructors
//-----------------------------------------------------------

System_Event_File::System_Event_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

System_Event_File::System_Event_File (char *filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	System_Event_File destructor
//-----------------------------------------------------------

System_Event_File::~System_Event_File (void)
{
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void System_Event_File::Setup (void)
{
	File_Type ("System Event File");
	File_ID ("SysEvent");

	time = type = node = plan = phase = ring = group = status = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool System_Event_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Add_Field ("TIME", STRING, TIME_BUFFER);
		Add_Field ("EVENT", INTEGER, 5);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("PLAN", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 10);
		Add_Field ("RING", INTEGER, 2);
		Add_Field ("GROUP", INTEGER, 2);
		Add_Field ("STATUS", STRING, 16);
	} else {
		Add_Field ("TIME", STRING, TIME_BUFFER);
		Add_Field ("EVENT", INTEGER, 5);
		Add_Field ("NODE", INTEGER, 10);
		Add_Field ("PLAN", INTEGER, 10);
		Add_Field ("PHASE", INTEGER, 5);
		Add_Field ("RING", INTEGER, 2);
		Add_Field ("GROUP", INTEGER, 2);
		Add_Field ("STATUS", STRING, 16);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool System_Event_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	time = Required_Field ("TIME");
	type = Required_Field ("EVENT", "TYPE");
	node = Required_Field ("NODE");

	if (!time || !type || !node) return (false);

	//---- optional fields ----

	plan = Optional_Field ("PLAN", "TIMING");
	phase = Optional_Field ("PHASE");
	ring = Optional_Field ("RING");
	group = Optional_Field ("GROUP");
	status = Optional_Field ("STATUS", "MESSAGE");

	return (true);
}
